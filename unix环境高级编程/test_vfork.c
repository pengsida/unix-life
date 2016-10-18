#include "apue.h"

int glob = 6;

int main()
{
    int var;
    pid_t pid;
    var = 88;
    printf("before vfork\n");
    if((pid = vfork()) < 0)
        err_sys("vfork error");
    else if(pid == 0)
    {
        ++glob;
        ++var;
        _exit(0); // 在此调用的是_exit，因为_exit并不执行I/O缓冲的冲洗操作。如果调用的是exit，则程序的输出是不确定的，它依赖于标准I/O库的实现。
    }
    printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
    exit(0);
}