#include <stdio.h>
#define DATAFILE "datafile"
#define BUFSIZ 1024
FILE* open_data(void)
{
    FILE *fp;
    char databuf[BUFSIZ];

    if((fp = fopen(DATAFILE, "r")) == NULL)
        return NULL;
    if(setvbuf(fp, databuf, _IOLBF, BUFSIZ) != 0)
        return NULL;
    return fp; // 当open_data函数返回时，它在栈上所使用的空间将由下一个被调用函数的栈帧使用。如果这里返回fp，那么标准I/O库函数仍将使用其流缓冲区的存储空间。 为了解决这个问题，我们应该将databuf设为全局变量或静态变量。或者为databuf动态地分配空间。
}