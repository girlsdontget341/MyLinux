#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>
void recylechild(int arg){
    // 写while是为了处理多个信号
    while(1){
        int tet = waitpid(-1, NULL, WNOHANG);
        if(tet == -1){
            break;// 所有子进程都回收了
        }else if(tet == 0){
            break;// 还有子进程活着
        }else{
            // 回收子进程
            printf("pid %d recyled\n", tet);
        }
    }
}
int main(){
    // 注册信号捕捉
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recylechild;
    sigaction(SIGCHLD, &act, NULL);

    int lfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(8888);
    saddr.sin_family = AF_INET;
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    listen(lfd, 128);

    while(1){
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        int cfd = accept(lfd,(struct sockaddr *)&cliaddr,&len);
        if(cfd == -1){
            // 用于处理信号捕捉导致的accept: Interrupted system call
            // 如果不加，子进程回收后父进程连接不到会终止，后续就再也连不上了
            if(errno == EINTR){
                continue;
            }
            perror("accept");
            exit(-1);
        }

        pid_t pid = fork();
        if(pid==0){
            char cliip[16];
            inet_ntop(PF_INET, &cliaddr.sin_addr.s_addr, cliip, 16);
            unsigned short cliport = ntohs(cliaddr.sin_port);
            printf("client ip is: %s, port is %d\n",cliip, cliport);

            char receive[1024];
            while(1){
                int res = read(cfd, receive, sizeof(receive));
                if(res==-1){
                    perror("read");
                    exit(-1);
                }else if(res>0){
                    printf("receive %s\n", receive);
                }else{
                    printf("client closed..\n");
                    break;// 退出循环，用来解决出现两次client closed
                }

                write(cfd, receive, strlen(receive)+1);
            }
            close(cfd);
            exit(0);
        }
    }
    close(lfd);
    return 0;
}