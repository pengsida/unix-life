#include "apue.h"
#include <errno.h>
#include <fcntl.h>

char buf[500000];

int main(void)
{
    int ntowrite, nwrite;
    char *ptr;

    // 从标准输入读500000字节
    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", ntowrite);
    // 设置非阻塞I/O
    set_fl(STDOUT_FILENO, O_NONBLOCK);
    ptr = buf;
    
    // 这个循环相当于一种轮询。程序发出了数千个write调用，系统不断轮询看是否输出数据。这种方式是浪费CPU时间的，可以采用多线程的方式替代。
    while(nrowrite > 0)
    {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        // 将每次write调用的结果在标准出错上打印
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
        if(nwrite > 0)
        {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }

    // 清除非阻塞标志
    clr_fl(STDOUT_FILENO, O_NONBLOCK);
    exit(0);
}