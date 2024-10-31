#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <bits/pthreadtypes.h>

int num=0;
pthread_rwlock_t rwlock;

void *writeth(void *arg){
    while(1){
        pthread_rwlock_wrlock(&rwlock);
        num++;
        printf("write from tid: %ld, num: %d\n",pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(500);
    }
}
void *readth(void *arg){
    while(1){
        pthread_rwlock_rdlock(&rwlock);
        printf("read from tid: %ld, num: %d\n",pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(500);
    }
}
int main(){
    pthread_rwlock_init(&rwlock,NULL);

    pthread_t wtid[3], rtid[5];
    for(int i=0; i<3; i++)
    {
        pthread_create(&wtid[i], NULL, writeth, NULL);
    }
    for(int i=0; i<5; i++)
    {
        pthread_create(&rtid[i], NULL, readth, NULL);
    }
    for(int i=0; i<3; i++)
    {
        pthread_detach(wtid[i]);
    }
        for(int i=0; i<5; i++)
    {
        pthread_detach(rtid[i]);
    }

    pthread_rwlock_destroy(&rwlock);

    pthread_exit(NULL);
    return 0;
}