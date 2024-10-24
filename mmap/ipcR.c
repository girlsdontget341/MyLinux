#include <stdio.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//实现没有关系的两个进程利用内存映射区进行通信 read
int main(){
    int fd = open("test2.txt", O_RDWR);
    int size = lseek(fd, 0, SEEK_END);
    void *ptrR = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    char buf[64];
    strcpy(buf, (char *) ptrR);
    printf("read data: %s", buf);
    munmap(ptrR, size);
}