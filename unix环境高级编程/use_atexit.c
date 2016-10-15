#include "apue.h"

// 静态函数会被自动分配在一个一直使用的存储区，直到退出应用程序实例，避免了调用函数时压栈出栈，速度快很多。
// 关键字“static”，译成中文就是“静态的”，所以内部函数又称静态函数。但此处“static”的含义不是指存储方式，而是指对函数的作用域仅局限于本文件。 使用内部函数的好处是：不同的人编写不同的函数时，不用担心自己定义的函数，是否会与其它文件中的函数同名，因为同名也没有关系。 
static void my_exit1(void);
static void my_exit2(void);

int main()
{
    if(atexit(my_exit2) != 0)
        err_sys("can't register my_exit2");
    if(atexit(my_exit1) != 0)
        err_sys("can't register my_exit1");
    
    printf("main is done\n");
    return 0;
}

static void my_exit1(void)
{
    printf("first exit handler\n");
}

static void my_exit2(void)
{
    printf("second exit handler\n");
}