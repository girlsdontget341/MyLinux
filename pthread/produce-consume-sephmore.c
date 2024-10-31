//完整版 信号量实现
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
pthread_mutex_t mutex;
struct node{
    int num;
    struct node *next;
};
sem_t psem;
sem_t csem;
struct node *head = NULL;
void *produce(void *arg){
    while(1){
        sem_wait(&psem);
        pthread_mutex_lock(&mutex);
        struct node *newnode = (struct node *)malloc(sizeof(struct node));
        newnode->num = rand()%1000;
        newnode->next = head;
        head = newnode;
        printf("add num: %d from tid: %ld \n",newnode->num, pthread_self());
        pthread_mutex_unlock(&mutex);
        sem_post(&csem);
    }
}

void *consume(void *arg){
    while(1){
        sem_wait(&csem);
        pthread_mutex_lock(&mutex);
        struct node *tmp =head;
        if(head!=NULL){
            head = head->next;
            printf("del num: %d from tid %ld\n", tmp->num,pthread_self());
            free(tmp);
            pthread_mutex_unlock(&mutex);
            sem_post(&psem);
        }
    }
}

int main(){
    pthread_mutex_init(&mutex, NULL);
    // 初始化信号量
    // 最多生产8个
    sem_init(&psem, 0, 8);
    // 初始没有东西可以消费
    sem_init(&csem, 0, 0);
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
    sem_destroy(&csem);
    sem_destroy(&psem);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);

    return 0;
}