#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SERVERIP "127.0.0.1"
#define PORT 9998

int main()
{
    // 1. 创建socket（用于通信的套接字）
    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd == -1) {
        perror("socket");
        exit(-1);
    }
    // 2. 连接服务器端
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    inet_pton(AF_INET, SERVERIP, &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(PORT);
    int ret = connect(connfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret == -1) {
        perror("connect");
        exit(-1);
    }
    // 3. 通信
    char recv_buf[1024] = {0};
    while (1) {
        // 发送数据
        char *send_buf = "client message";
        write(connfd, send_buf, strlen(send_buf));
        // 接收数据
        ret = read(connfd, recv_buf, sizeof(recv_buf));
        if (ret == -1) {
            perror("read");
            exit(-1);
        } else if (ret > 0) {
            printf("recv server data : %s\n", recv_buf);
        } else {
            // 表示客户端断开连接
            printf("client closed...\n");
        }
        // 休眠的目的是为了更好的观察，放在此处可以解决read: Connection reset by peer问题
        sleep(1);
    }
    // 关闭连接
    close(connfd);
    return 0;
}