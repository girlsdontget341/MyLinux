/*
    使用条件变量升级
    当消费者发现链表为空时，可以调用 pthread_cond_wait(&cond, &mutex);等待，直到生产者通过 pthread_cond_signal(&cond); 或 pthread_cond_broadcast(&cond); 发出信号，通知有新节点可供消费。
    生产者每次添加新节点后，通过条件变量通知等待的消费者去处理节点，避免消费者反复尝试获取锁。
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_cond_t cond;
pthread_mutex_t mutex;
struct node{
    int num;
    struct node *next;
};
struct node *head = NULL;
void *produce(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        struct node *newnode = (struct node *)malloc(sizeof(struct node));
        newnode->num = rand()%1000;
        newnode->next = head;
        head = newnode;
        printf("add num: %d from tid: %ld \n",newnode->num, pthread_self());

        // 只要生产了一个，就通知消费者消费
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        usleep(100);
    }
}

void *consume(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        struct node *tmp =head;
        if(head!=NULL){
            head = head->next;
            printf("del num: %d from tid %ld\n", tmp->num,pthread_self());
            free(tmp);
            pthread_mutex_unlock(&mutex);
            usleep(100);
        }else{
            // 没有数据，需要等待
            // 当这个函数调用阻塞的时候，会对互斥锁进行解锁，当不阻塞的，继续向下执行，会重新加锁。
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
}

int main(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_t producer[5], consumer[5];
    for(int i=0; i<5; i++)
    {
        pthread_create(&producer[i], NULL, produce, NULL);
        pthread_create(&consumer[i], NULL, consume, NULL);
    }
    for(int i=0; i<5; i++)
    {
        pthread_detach(producer[i]);
        pthread_detach(consumer[i]);
    }
    while (1) {//必须加 防止在线程结束之前锁被释放
        sleep(10);
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);

    return 0;
}