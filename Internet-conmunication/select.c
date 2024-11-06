#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
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

    //创建需要检测的文件描述符的集合
    fd_set rdset, tmp;
    FD_ZERO(&rdset);
    FD_SET(lfd, &rdset);

    int maxfd = lfd;//暂时设置最大文件描述符是lfd，后续若有客户端连进来再随时更新
    while(1)
    {   
        tmp = rdset;
        int rets = select(maxfd+1, &tmp, NULL, NULL, NULL);//调用select，只检测哪些文件描述符读到了数据，设置为永久阻塞
        if(rets == -1){
            perror("select");
            exit(-1);
        }else if(rets == 0){
            continue;//此时因为设置永久阻塞，ret不可能为0（ret返回的是有变化的文件描述符的数量），但如果设置阻塞时间，到时间了客户端还没连进来，ret为0
        }else if(rets > 0){
            if(FD_ISSET(lfd,&tmp)){//socket的文件描述符变了，说明有连进来了
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);//通信文件描述符
                // 输出客户端信息，IP组成至少16个字符（包含结束符）
                char client_ip[16] = {0};
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, client_ip, sizeof(client_ip));
                unsigned short client_port = ntohs(cliaddr.sin_port);
                printf("ip:%s, port:%d\n", client_ip, client_port);
                FD_SET(cfd, &rdset);//新文件描述符加入集合
                maxfd = maxfd > cfd ? maxfd : cfd;
            }

            for(int i = lfd + 1;i<=maxfd;i++)
            {
                if(FD_ISSET(i,&tmp)){
                    char buf[1024]={0};
                    int ret = read(i, buf, sizeof(buf));
                    if(ret == -1){
                        perror("read");
                        exit(-1);
                    }else if(ret == 0){//客户端关闭，把对应文件描述符清除
                        printf("client closed..\n");
                        close(i);
                        FD_CLR(i, &rdset);
                    }else if(ret > 0)
                    {
                        printf("read data: %s\n",buf);
                        write(i, "back from server", 20);
                    }
                }
            }
        }
    }
    close(lfd);

    return 0;
}