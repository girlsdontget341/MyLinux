#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"
#include <ctype.h>
#include <signal.h>
#include <cassert>

#define MAX_FD 65536 //最大文件描述符数量
#define MAX_EVENT_NUMBER 10000 //监听的最大事件数量

extern void addfd(int epollfd, int fd, bool one_shot);
extern void removefd(int epollfd, int fd);

int isValidPort(const char* port){//检验输入是否为有效端口号
    for(int i = 0; port[i] != '\0'; i++)//?""
    {
        if(!isdigit(port[i])){
            return 0;
        }
    }
    int port_num = atoi(port);
    return port_num > 1 && port_num < 65535;
}

void addsig(int sig, void(handler)(int)){//捕捉信号（函数指针）
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));//初始化结构体
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    //屏蔽所有信号（在信号处理函数执行期间，所有其他信号都会被阻塞，防止信号嵌套（即同一信号重复触发或其他信号干扰处理过程））
    assert( sigaction(sig, &sa, NULL) != -1);
    //用于在运行时检查一个条件是否为真，如果条件不满足，则运行时将终止程序的执行并输出一条错误信息
}

int main(int argc, char* argv[]){//前面表示命令行参数个数 后面是字符串指针表示命令
    if(argc <= 1){//说明只输入了1个参数 缺少端口号
        printf("usage: %s port_number\n", basename(argv[0]));
        return 1;
    }
    // if(!isValidPort(argv[1])){//检验端口号是否合法
    //     printf("Error: Invalid port number '%s'. Please provide a number between 1 and 65535.\n", argv[1]);
    //     return 1;
    // }

    int port = atoi(argv[1]);
    addsig(SIGPIPE, SIG_IGN);//忽略SIGPIPE信号

    threadpool<http_conn> * pool = NULL;
    try{
        pool = new threadpool<http_conn>;
    }catch(...){//表示捕获所有异常
        return 1;
    }

    http_conn* users = new http_conn[MAX_FD];//存储客户端的信息
    int listenfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    int reuse = 1;//设置端口复用
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int ret = 0;
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    ret = listen(listenfd, 5);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

    while(1){
        //来等待并处理 epoll 实例中已就绪的事件
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if((num < 0) && (errno != EINTR)){
            //errno != EINTR：EINTR 是一个错误代码，表示系统调用被信号中断。
            //如果 errno 等于 EINTR，这通常意味着 epoll_wait() 被一个信号中断，
            //可以忽略这个错误并重新调用 epoll_wait()。
            printf("epoll failure\n");
            break;
        }

        for(int i = 0; i<num; i++){
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd){
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                if(connfd < 0){
                    printf( "errno is: %d\n", errno );
                    continue;
                }
                if(http_conn::m_user_count >= MAX_FD){//连接数超过限制
                    close(connfd);
                    continue;
                }
                users[connfd].init(connfd, client_address);
            }else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                /*EPOLLRDHUP：这个事件表示远程端已经关闭连接（如，TCP连接的对端发送了 FIN）。它通常意味着客户端关闭了连接。
                EPOLLHUP：该事件表示连接被挂起，通常意味着远端连接被断开或连接出现了问题。
                EPOLLERR：表示套接字发生错误。
                这个条件判断用于处理以下情况：
                如果发生了上述事件之一，表示客户端的连接已经出错或关闭。
                在这种情况下，我们需要关闭连接。*/
                users[sockfd].close_conn();
            }else if(events[i].events & EPOLLIN){
                if(users[sockfd].read()){
                    pool->append(users + sockfd);
                }else{
                    users[sockfd].close_conn();
                }
            }else if(events[i].events & EPOLLOUT){
                if(!users[sockfd].write()){
                    users[sockfd].close_conn();
                }
            }
        }
    }
    close(epollfd);
    close(listenfd);
    delete [] users;
    delete pool;
    return 0;
}
