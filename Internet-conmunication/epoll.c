//使用epoll方法实现服务器，客户端同select_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/time.h> 
#include <sys/types.h> 

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

    //创建epoll实例，将lfd添加到其中
    int epfd = epoll_create(100);
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    struct epoll_event epevs[1024];
    while(1){
        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }
        printf("%d fds has changed..\n", ret);

        for(int i = 0; i<ret; i++){
            int curfd = epevs[i].data.fd;
            if(curfd == lfd){//客户端连接
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);//通信文件描述符
                // 输出客户端信息，IP组成至少16个字符（包含结束符）
                char client_ip[16] = {0};
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, client_ip, sizeof(client_ip));
                unsigned short client_port = ntohs(cliaddr.sin_port);
                printf("ip:%s, port:%d\n", client_ip, client_port);

                epev.data.fd = cfd;
                epev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            }else{//数据到达
                char buf[1024]={0};
                int retu = read(curfd, buf, sizeof(buf));
                if(retu == -1){
                    perror("read");
                    exit(-1);
                }else if(retu == 0){//客户端关闭，把对应文件描述符清除
                    printf("client closed..\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }else if(retu > 0)
                {
                    printf("read data: %s\n",buf);
                    write(curfd, "back from server", 20);
                }
            }
        }
    }
    close(lfd);
    close(epfd);
    return 0;
}