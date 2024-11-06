#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/time.h> 
#include <sys/types.h> 
#include <poll.h>

int main(){
    //create socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9998);

    //bind
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    //listen
    listen(lfd, 128);

    struct pollfd fds[1024];
    for(int i = 0; i < 1024; i++)//初始化poll的文件检测符数组
    {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;

    int nfds = 0;//后续若有客户端连进来再随时更新索引
    while(1)
    {   
        int rets = poll(fds, nfds + 1, -1);
        if(rets == -1){
            perror("poll");
            exit(-1);
        }else if(rets == 0){
            continue;
        }else if(rets > 0){
            if(fds[0].revents & POLLIN){//socket的文件描述符变了，说明有连进来了(!这里只能用 与 来判断！)
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);//通信文件描述符
                // 输出客户端信息，IP组成至少16个字符（包含结束符）
                char client_ip[16] = {0};
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, client_ip, sizeof(client_ip));
                unsigned short client_port = ntohs(cliaddr.sin_port);
                printf("ip:%s, port:%d\n", client_ip, client_port);
                //新文件描述符加入集合
                for(int i = 1;i < 1024; i++)
                {
                    if(fds[i].fd == -1){
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
                //更新最大文件描述符索引
                nfds = nfds > cfd? nfds: cfd;
            }

            for(int i = 1;i<= nfds;i++)
            {
                if(fds[i].revents & POLLIN){
                    char buf[1024]={0};
                    int ret = read(fds[i].fd, buf, sizeof(buf));
                    if(ret == -1){
                        perror("read");
                        exit(-1);
                    }else if(ret == 0){//客户端关闭，把对应文件描述符清除
                        printf("client closed..\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    }else if(ret > 0)
                    {
                        printf("read data: %s\n",buf);
                        write(fds[i].fd, "back from server", 20);
                    }
                }
            }
        }
    }
    close(lfd);

    return 0;
}