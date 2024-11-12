#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BROADCASTIP "192.168.206.255"
#define PORT 6789

int main()
{
    // 1. 创建通信套接字
    int connfd = socket(PF_INET, SOCK_DGRAM, 0);

    // 2.设置广播属性
    int op = 1;
    setsockopt(connfd, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op));

    // 3.创建一个广播的地址
    struct sockaddr_in broad_addr;
    broad_addr.sin_family = AF_INET;
    broad_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, BROADCASTIP, &broad_addr.sin_addr.s_addr);

    // 4. 通信
    int num = 0;
    while (1) {
        char sendBuf[128];
        sprintf(sendBuf, "hello, client....%d", num++);
        // 发送数据
        sendto(connfd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr *)&broad_addr, sizeof(broad_addr));
        printf("广播的数据：%s\n", sendBuf);
        sleep(1);
    }
    close(connfd);
    return 0;
}