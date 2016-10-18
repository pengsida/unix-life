#include "apue.h"
#include <sys/wait.h>

int main(void)
{
    pid_t pid;

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(pid == 0)
    {
        if((pid = fork()) < 0)
            err_sys("fork error");
        else if(pid > 0)
        {
            sleep(2);
            exit(0);    // 相当于让第一个子进程exit
        }
        
        // 接下来的代码段只有第二个子进程能够执行到
        // 注意，因为第二个子进程的父进程已经exit，则它的父进程变为init进程
        // 所以第二个子进程此时不会变为僵死进程

        // sleep(2); // 这里可能产生一个竞争条件。所以调用sleep等待第一个子进程先运行。
        // 有效地解决此处竞争条件的方法如下
        while(getppid() != 1)
            sleep(1);
        // 以上的方法称为轮询。它的缺点是这里浪费了CPU时间，因为调用者每隔一秒就被唤醒，然后进行条件测试。
        // 为了避免竞争条件和轮询，在多个进程之间需要有某种形式的信号发送和接收的方法。在UNIX中可以使用信号机制，也可以使用各种形式的进程通信IPC。
        printf("second child, parent id = %d\n",getppid());
        exit(0);
    }

    if(waitpid(pid, NULL, 0) != pid) // 等待第一个子进程
        err_sys("waitpid error");
    
    exit(0);
}