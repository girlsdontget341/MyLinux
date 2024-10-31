/*
    丐版生产者消费者，只加了一个互斥锁
    能执行，但会浪费资源
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

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
            usleep(100);
            pthread_mutex_unlock(&mutex);
        }
    }
}

int main(){
    pthread_mutex_init(&mutex, NULL);
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
    //必须加 防止在线程结束之前锁被释放
    while (1) {
        sleep(10);
    }
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);

    return 0;
}