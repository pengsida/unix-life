#include "apue.h"
#include <fcntl.h>

static void lockabyte(const char *name, int fd, off_t offset)
{
    if(write_lock(fd, offset, SEEK_SET, 1) < 0)
        err_sys("%s: writew_lock error", name);
    printf("%s: got the lock, byte %lld\n", name, offset);
}


// 当两个进程相互等待对方持有并且锁定的资源时，则这两个进程就处于死锁状态。
int main(void)
{
    int fd;
    pid_t pid;

    if((fd = creat("templock", FILE_MODE)) < 0)
        err_sys("creat error");
    if(write(fd, "ab", 2) != 2)
        err_sys("write error");
    
    TELL_WAIT();
    if((pid = fork()) < 0)
        err_sys("fork error");
    else if(pid == 0)
    {
        // 子进程锁字节0
        lockabyte("child", fd, 0);
        TELL_PARENT(getppid());
        // 等待父进程获得它设置的第一把锁
        WAIT_PARENT();
        lockabyte("child", fd, 1);
    }
    else
    {
        // 父进程锁字节1
        lockabyte("parent", fd, 1);
        TELL_CHILD(pid);
        // 等待子进程获得它设置的第一把锁
        WAIT_CHILD();
        lockabyte("parent", fd, 0);
    }
    exit(0);
}