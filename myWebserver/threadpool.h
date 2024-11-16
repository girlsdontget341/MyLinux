#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"

// 线程池类，将它定义为模板类是为了代码复用，模板参数T是任务类
template<typename T>
class threadpool
{
private:
    int m_thread_number; //线程数量
    pthread_t * m_threads; //线程数组
    int m_max_requests; //队列允许的最大请求数量
    std::list<T *> m_work_queue; //请求队列
    locker m_queuelocker; //队列的锁
    sem m_queuestat; //处理队列的信号量
    bool m_stop; //结束线程
public:
    threadpool(int thread_number = 8, int max_requests = 10000);
    ~threadpool();
    bool append(T* request);
private:
    static void* worker(void* arg);
    void run();
};

template< typename T >
threadpool<T>::threadpool(int thread_number, int max_requests)://构造函数中，使用初始化列表初始化了类中的成员变量
    m_thread_number(thread_number), m_max_requests(max_requests),
    m_stop(false), m_threads(NULL)
{
    if((thread_number <= 0) || (max_requests <= 0) ) {
        throw std::exception();
    }

    m_threads = new pthreads_t[m_thread_number];
    //m_threads = new pthread_t[thread_number];  // 错误：使用局部参数
    //这将导致问题，因为 thread_number 仅在构造函数内部有效，一旦构造函数结束，thread_number 的值就会消失。
    //m_threads 数组的大小需要基于类的成员变量 m_thread_number，以便确保它在整个对象生命周期内有效。
    if(!m_threads){
        throw std::exception();
    }

    for(int i = 0; i < m_thread_number; i++){
        printf( "create the %dth thread\n", i);
        if(pthread_create(m_threads + i, NULL, worker, this) != 0){//传递当前线程池对象的指针给工作线程，以便线程能够访问 threadpool 类的成员。
            delete [] m_threads;//创建失败 释放空间
            throw std::exception();
        }

        if(pthread_detach(m_threads[i])){//设置为分离线程 能够自动回收资源
            delete [] m_threads; //设置失败
            throw std::exception();
        }
    }
};

template< typename T >
threadpool<T>::~threadpool()
{
    delete [] m_threads;
    m_stop = true;
};

template<typename T>
bool threadpool<T>::append(T *request){
    //// 操作工作队列时一定要加锁，因为它被所有线程共享。
    m_queuelocker.lock();
    if(m_work_queue.size() > m_max_requests){//当前队列大小超过限制
        m_queuelocker.unlock();
        return false;
    }
    m_work_queue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();//通知线程来活了
    return true;
};

template<typename T>//调用run函数
void * threadpool<T>::worker(void *arg){
    // 使用 this 指针直接访问当前 threadpool 对象
    // this->run();
    // return this;
    threadpool* pool = ( threadpool* )arg;
    pool->run();
    return pool;
};

template<typename T>
void threadpool<T>::run(){
    while(!m_stop){//线程未终止就继续
        m_queuestat.wait();//P
        m_queuelocker.lock();//上锁
        if(m_work_queue.empty()){//如果工作队列为空，释放锁并继续下一轮循环
            m_queuelocker.unlock();
            continue;
        }
        T * request = m_work_queue.front();
        m_work_queue.pop_front();
        m_queuelocker.unlock();
        if(!request){
            continue;
        }
        request->process();//？暂时不知道process哪来的
    }
}

#endif
