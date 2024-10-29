#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
int main(){
    //获取共享内存
    int shmid = shmget(100, 0, IPC_CREAT);
    printf("shmid is: %d\n", shmid);
    //关联进程
    void *ptr = shmat(shmid, NULL, 0);

    //read
    printf("%s\n", (char *)ptr);

    //hold on
    printf("wait a moment..");
    getchar();

    //解除关联
    shmdt(ptr);

    //delete shared memory
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}