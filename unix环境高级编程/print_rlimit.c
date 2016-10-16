#include "apue.h"
#if defined(BSD) || defined(MACOS)
#include<sys/time.h>
#define FMT "%10ld  "
#else
#define FMT "%10lld  "
#endif
#include<sys/resource.h>

#define doit(name) pr_limits(#name, name)
// 在doit宏中使用ISO C的字符串创建运算符#，用于为每个资源名产生字符串值

static void pr_limits(char *, int);

int main(void)
{
    #ifdef RLIMIT_AS
        doit(RLIMIT_AS); // 进程可用存储区的最大总长度
    #endif
    
    doit(RLIMIT_CORE); // core文件的最大字节数。如果值为0则阻止创建core文件
    doit(RLIMIT_CPU); // CPU时间的最大量值
    doit(RLIMIT_DATA); // 数据段的最大字节长度
    doit(RLIMIT_FSIZE); // 可以创建的文件的最大字节长度

    #ifdef RLIMIT_MEMLOCK
        doit(RLIMIT_MEMLOCK);  // 一个进程使用mlock(2)能锁定在存储器中的最大字节长度
    #endif
    
    doit(RLIMIT_NOFILE);  // 每个进程能打开的最大文件数

    #ifdef RLIMIT_NPROC
        doit(RLIMIT_NPROC); // 每个实际用户ID可拥有的最大字进程数
    #endif

    #ifdef RLIMIT_SBSIZE
        doit(RLIMIT_SBSIZE); // 最大驻内存集的字节长度
    #endif

    #ifdef RLIMIT_VMEM
        doit(RLIMIT_VMEM) // 用户可以占用的套接字缓冲区的最大长度
    #endif

    exit(0);
} 

static void pr_limits(char *name, int resource)
{
    struct rlimit limit;
    if(getrlimit(resource, &limit) < 0)
        err_sys("getrlimit error for %s", name);
    printf("%-14s  ", name);
    if(limit.rlim_cur == RLIM_INFINITY)
        printf("(infinite)  ");
    else
        printf(FMT, limit.rlim_cur);
    if(limit.rlim_max == RLIM_INFINITY)
        printf("(infinite)  ");
    else
        printf(FMT, limit.rlim_max);
    putchar((int)'\n');
}