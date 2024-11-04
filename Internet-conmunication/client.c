//TCP通信的客户端

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(){
    //1. create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    //2.connect
    struct sockaddr_in serveraddr;
    inet_pton(AF_INET, "192.168.206.130", &serveraddr.sin_addr.s_addr);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(9999);
    connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    //3.conmunication
    char data[256];
    scanf("%s", data);//data from keyboad
    write(fd, data, strlen(data));

    char receive[1024];
    int len = read(fd, receive, sizeof(receive));
    if(len == -1){
        perror("read");
        exit(0);
    }else if(len > 0){
        printf("receive server data is :%s\n",receive);
    }else if(len == 0)
    {
        printf("server closed..\n");
    }

    close(fd);
}