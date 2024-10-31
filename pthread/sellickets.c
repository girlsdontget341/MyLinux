/*
    多线程实现卖票
    3窗口 100张票
    有问题，没设置临界区，可能导致线程不同步
*/

#include <stdio.h>
#include <pthread.h>

int tickets = 100;
void *selltickets(void *arg){
    
    while(tickets>0){
        printf("%ld is selling %d ticket\n", pthread_self(), tickets);
        tickets--;
    }
}

int main(){
    pthread_t tid1,tid2,tid3;
    pthread_create(&tid1, NULL, selltickets, NULL);
    pthread_create(&tid2, NULL, selltickets, NULL);
    pthread_create(&tid3, NULL, selltickets, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    // pthread_detach(tid1);
    // pthread_detach(tid2);
    // pthread_detach(tid3);

    pthread_exit(NULL);
    return 0;
}