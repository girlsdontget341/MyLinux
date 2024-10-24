#include <stdio.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
    int fd = open("english.txt", O_RDWR);

    int fd1 = open("cpy.txt", O_RDWR |O_CREAT, 0664);

    int size = lseek(fd, 0, SEEK_END);

    truncate("cpy.txt", size);

    void *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    void *ptr1 = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
    if (ptr == MAP_FAILED || ptr1 == MAP_FAILED)
    {
        perror("mmap");
        exit(0);
    }

    memcpy(ptr1, ptr, size);

    munmap(ptr, size);
    munmap(ptr1, size);

    close(fd1);
    close(fd);
}