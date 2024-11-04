//TCP通信的客户端

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(){
    //1. create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //2.connect
    struct sockaddr_in serveraddr;
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
    //serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8888);
    int ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ret == -1){
        perror("connect");
        exit(-1);
    }

    //3.conmunication
    int i = 0;
    char data[256];
    while(1){
        sprintf(data,"data : %d\n",i++);
        write(fd, data, strlen(data)+1);

        int len = read(fd, data, sizeof(data));
        if(len == -1){
            perror("read");
            exit(0);
        }else if(len > 0){
            printf("receive server data is :%s\n",data);
        }else if(len == 0)
        {
            printf("server closed..\n");
        }
        sleep(1);
    }
    close(fd);
}