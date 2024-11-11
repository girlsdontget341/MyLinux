//使用epoll方法实现服务器，客户端同select_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/time.h> 
#include <sys/types.h> 
#include <fcntl.h>

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

                int flag = fcntl(cfd, F_GETFL);//设置非阻塞
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);
                epev.data.fd = cfd;
                epev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            }else{//数据到达
                if(epevs[i].events & EPOLLOUT){
                    continue;
                }
                //一次性读出所有数据（循环）
                char buf[5];
                int lent = 0;
                while((lent = read(curfd, buf, sizeof(buf)))>0){
                    printf("recv data %s\n", buf);
                    write(curfd, buf, sizeof(buf));
                    memset(buf, 0, sizeof(buf));
                }
                if(lent == 0){
                    printf("client closed..\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);  // 从 epoll 中移除
                    close(curfd);  // 关闭文件描述符
                    break;
                }
            }
        }
    }
    close(lfd);
    close(epfd);
    return 0;
}