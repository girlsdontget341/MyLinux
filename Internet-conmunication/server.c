//TCP通信的服务器端

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
int main()
{
    //1.create socket for listening
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    //2.bind
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.206.130", &saddr.sin_addr.s_addr);
    // saddr.sin_addr.s_addr = INADDR_ANY //0.0.0.0 presents any address
    saddr.sin_port = htons(9999);
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    //3.listen
    listen(lfd, 8);

    //4.accept
    struct sockaddr_in clientaddr;
    socklen_t size = sizeof(clientaddr);
    int cfd = accept(lfd, (struct sockaddr *)&clientaddr, &size);//cfd is a fd for communication

    //printf client information
    char client[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, client, 16);
    unsigned short clientport = ntohs(clientaddr.sin_port);
    printf("client IP is : %s, port is :%d\n", client, clientport);
    
    //5.conmunication
    char receive[1024];
    int len = read(cfd, receive, sizeof(receive));
    if(len == -1){
        perror("read");
        exit(0);
    }else if(len > 0){
        printf("receive data is :%s\n",receive);
    }else if(len == 0)
    {
        printf("client closed..\n");
    }

    write(cfd, receive, strlen(receive));

    //close
    close(cfd);
    close(lfd);
}