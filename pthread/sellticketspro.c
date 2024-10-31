#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;//全局锁变量
int tickets = 500;
void *selltickets(void *arg){
    
    while(1){
        //访问前加锁
        pthread_mutex_lock(&mutex);
        if(tickets > 0){
            printf("%ld is selling %d ticket\n", pthread_self(), tickets);
            tickets--;
        }else{
            //解锁
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    pthread_mutex_init(&mutex, NULL);

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

    pthread_mutex_destroy(&mutex);

    return 0;
}