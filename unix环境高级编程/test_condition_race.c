#include "apue.h"

// int main()
// {
//     TELL_WAIT();

//     if((pid = fork()) < 0)
//         err_sys("fork error");
//     else if(pid == 0)
//     {
//         TELL_PARENT(getppid()); // 通知父进程自己已经完成工作
//         WAIT_PARENT(); // 等待父进程完成
//         exit(0);
//     }

//     TELL_CHILD(pid); // 通知子进程自己已经完成工作
//     WAIT_CHILD(); // 等待子进程完成
//     exit(0);
// }

static void charatatime(char *);

int main(void)
{
    pid_t pid;

    TELL_WAIT(); // set things up for TELL_xxx & WAIT_xxx

    if((pid = fork()) < 0)
        err_sys("fork error");
    // 以下程序让父进程先运行
    // else if(pid == 0)
    // {
    //     WAIT_PARENT();
    //     charatatime("output from child\n");
    // }
    // else
    // {
    //     charatatime("output from parent\n"); // 这里有潜在的竞争条件
    //     TELL_CHILD(pid); // 与WAIT_PARENT函数相呼应。这两个函数作用解决了竞争条件
    // }
    // 以下程序让子进程先运行
    else if(pid == 0)
    {
        charatatime("output from child\n");
        TELL_PARENT(pid);
    }
    else
    {
        WAIT_CHILD();
        charatatime("output from parent\n");
    }
    exit(0);
}

static void charatatime(char *str)
{
    char *ptr;
    int c;
    setbuf(stdout, NULL);
    for(ptr = str; (c = *ptr++) != 0;)
        putc(c, stdout);
}