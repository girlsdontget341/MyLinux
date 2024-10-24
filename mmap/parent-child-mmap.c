#include <stdio.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(){
    int fd = open("test.txt", O_RDWR);//打开一个文件

    int size = lseek(fd, 0, SEEK_END);//返回文件大小
    //创建内存映射区
    void *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        exit(0);
    }

    pid_t pid = fork();
    if(pid == 0 ){
        strcpy((char *)ptr, "sb play genshin!");
    }else{
        wait(NULL);
        char buf[128];
        strcpy(buf, (char *)ptr);
        printf("read data： %s", buf);
    }

    munmap(ptr, size);//关闭内存映射区
    return 0;
}