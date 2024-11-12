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

    // 2.客户端绑定通信的IP和端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    // 设置为接收任意网址信息或指定多播地址
    // addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, BROADCASTIP, &addr.sin_addr.s_addr);

    // 3. 将信息进行绑定
    int ret = bind(connfd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }
    // 4. 通信
    while (1) {
        char buf[128];
        // 接收数据
        int num = recvfrom(connfd, buf, sizeof(buf), 0, NULL, NULL);
        printf("server say : %s\n", buf);
    }
    close(connfd);
    return 0;
}