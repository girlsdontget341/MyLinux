#include <stdio.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <bits/mman-linux.h>
//匿名映射 实现进程通信（只能在有关系的进程进行）
int main(){
    int size = 4096;
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        exit(0);
    }

    pid_t pid = fork();
    if(pid>0){
        strcpy((char *)ptr, "sbrm");
        wait(NULL);//回收子进程资源
    }else if(pid ==0){
        sleep(1)//防止子进程先执行，读不到数据
        printf("%s\n", (char *)ptr);
    }else{
        perror("fork");
        exit(0);
    }

    munmap(ptr, size);
}