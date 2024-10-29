/*
用一个守护进程，每隔两秒获取系统时间，写入磁盘文件
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void work(int num){
    time_t tm = time(NULL);
    struct tm *loc = localtime(&tm);
    // char buf[1024];
    // sprintf(buf, "%d-%d-%d %d:%d:%d\n", loc->tm_year,loc->tm_mon, loc->tm_mday, loc->tm_hour, loc->tm_min, loc->tm_sec);
    // printf("%s\n", buf);

    char *str = asctime(loc);
    int fd = open("time.txt", O_CREAT|O_RDWR|O_APPEND, 0664);
    write(fd, str, strlen(str));
    close(fd);
}

int main(){
    //fork()
    pid_t pid = fork();

    if(pid > 0){
        exit(0);
    }
    //子进程创建会话
    setsid();
    //设置掩码
    umask(022);
    //更改工作目录
    chdir("/home/nowcoder/");
    //关闭、重定向文件描述符
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = work;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM, &act, NULL);


    struct itimerval val;
    val.it_value.tv_sec = 2;
    val.it_value.tv_usec = 0;
    val.it_interval.tv_sec = 2;
    val.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &val, NULL);

    while(1){
        sleep(10);
    }

    return 0;
}