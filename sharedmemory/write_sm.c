#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
int main(){
    //创建共享内存
    int shmid = shmget(100, 4096, IPC_CREAT|0664);
    printf("shmid is: %d\n", shmid);
    //关联进程
    void *ptr = shmat(shmid, NULL, 0);

    //write
    char *s = "sbwrfcnmjsn";
    memcpy(ptr, s, strlen(s)+1);

    //hold on
    printf("wait a moment..");
    getchar();

    //解除关联
    shmdt(ptr);

    //delete shared memory
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}