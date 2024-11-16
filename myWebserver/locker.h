#ifndef LOCKER_H
#define LOCKER_H
//线程同步机制的封装
#include <exception>
#include <pthread.h>
#include <semaphore.h>

//互斥锁
class locker {
public:
    locker(){//构造
        if(pthread_mutex_init(&m_mutex, NULL) != 0){
            throw std::exception();
        }
    }

    ~locker(){//析构
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock(){//上锁
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock(){//解锁
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    pthread_mutex_t * get(){//获取原始锁变量的指针
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

//条件变量类
class cond {
public:
    cond(){
        if(pthread_cond_init(&m_cond, NULL) != 0){
            throw std::exception();
        }
    }

    ~cond(){
        pthread_cond_destroy(&m_cond);
    }

    bool wait(pthread_mutex_t m_mutex){//使当前线程等待条件变量 m_cond
        return pthread_cond_wait(&m_cond, &m_mutex) == 0;
    }

    bool timewait(pthread_mutex_t m_mutex, struct timespec t){//使当前线程限时等待条件变量 m_cond，超时返回
        return pthread_cond_timedwait(&m_cond, &m_mutex, &t) == 0;
    }

    bool signal(){//唤醒一个等待该条件变量的线程
        return pthread_cond_signal(&m_cond) == 0;
    }

    bool broadcast(){//唤醒全部等待该条件变量的线程
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    pthread_cond_t m_cond;
};

//信号量类
class sem {
public:
    sem(){
        if(sem_init(&m_sem, 0, 0) != 0){//第二个参数为0表示线程间共享
            throw std::exception();
        }
    }

    sem(int num){//重载构造函数 对应信号量初始值情况
        if(sem_init(&m_sem, 0, num) != 0){
            throw std::exception();
        }        
    }

    ~sem(){
        sem_destroy(&m_sem);
    }

    bool wait(){//P操作
        return sem_wait(&m_sem) == 0;
    }

    bool post(){//V操作
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};
#endif