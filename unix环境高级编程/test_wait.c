#include "apue.h"
#include<sys/wait.h>

void pr_exit(int status)
{
    if(WIFEXITED(status))
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n",WTERMSIG(status),
        #ifdef WCOREDUMP
            WCOREDUMP(status) ? "(core file generated)" : "");
        #else
            "");
        #endif
    else if(WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

int main(void)
{
    pid_t pid;
    int status;

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(pid == 0)
        exit(7);
    
    if(wait(&status) != pid) // 如果一个进程有几个子进程，那么只要有一个子进程终止，wait就返回。如果要等待一个指定的进程终止，需要调用wait，然后将其返回的进程ID和所期望的进程ID相比较。如果终止进程不是所期望的，则将该进程ID和终止状态保存起来，然后再次调用wait。反复如此知道所期望的进程终止。
        err_sys("wait error");
    pr_exit(status);

    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(pid == 0)
        abort();
    
    if(wait(&status) != pid)
        err_sys("wait error");
    pr_exit(status);

    // if((pid = fork()) < 0)
    //     err_sys("fork error");
    // else if(pid == 0)
    //     status /= 0;
    
    // if(wait(&status) != pid)
    //     err_sys("wait error");
    // pr_exit(status);

    exit(0);
}