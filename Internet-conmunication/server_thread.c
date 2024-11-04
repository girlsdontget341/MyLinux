//多线程实现tcp通信服务器端
//客户端可同上
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

struct sockinfo{
    int fd; //文件描述符
    pthread_t tid; //线程号
    struct sockaddr_in addr; //客户端信息
};
struct sockinfo sockinfos[128];     // 表示最大有128个客户端连接

void *working(void *arg){
    //子线程和客户端通信
    struct sockinfo *pinfo = (struct sockinfo *)arg;
    char cliip[16];
    inet_ntop(PF_INET, &pinfo->addr.sin_addr.s_addr, cliip, 16);
    unsigned short cliport = ntohs(pinfo->addr.sin_port);
    printf("client ip is: %s, port is %d\n",cliip, cliport);

    char receive[1024];
    while(1){
        int res = read(pinfo->fd, receive, sizeof(receive));
        if(res==-1){
            perror("read");
            exit(-1);
        }else if(res>0){
            printf("receive %s\n", receive);
        }else{
            printf("client closed..\n");
            break;// 退出循环，用来解决出现两次client closed
        }

        write(pinfo->fd, receive, strlen(receive)+1);
    }
    close(pinfo->fd);
    exit(0);
}
int main(){
    // 初始化线程结构体数据
    int sockinfo_maxLen = sizeof(sockinfos) / sizeof(sockinfos[0]);
    for (int i = 0; i < sockinfo_maxLen; i++) {
        bzero(&sockinfos[i], sizeof(sockinfos[i]));
        sockinfos[i].fd = -1;
        sockinfos[i].tid = -1;
    }

    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in saddr;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(8888);
    saddr.sin_family = AF_INET;
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1){
        perror("bind");
        exit(-1);
    }

    ret = listen(lfd, 128);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    while(1){
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        int cfd = accept(lfd,(struct sockaddr *)&cliaddr,&len);

        struct sockinfo *pinfo;//结构体指针
        // 从结构体数组中找到一个可用的元素进行赋值
        for(int i=0;i<sockinfo_maxLen;i++)
        {
            if(sockinfos[i].fd==-1)
            {
                pinfo = &sockinfos[i];
                break;
            }
            // 当遍历到最后还没有找到，那么休眠一秒后，继续尝试
            if(i==sockinfo_maxLen-1)
            {
                sleep(1);
                i--;
            }
        }
        pinfo->fd=cfd;
        memcpy(&pinfo->addr, &cliaddr, sizeof(cliaddr));
        //创建子线程实现通信
        pthread_create(&pinfo->tid, NULL,working, pinfo);

        pthread_detach(pinfo->tid);
    }
    close(lfd);
    return 0;
}