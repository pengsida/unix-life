unix环境高级编程第七章
{
    7.1 本章学习内容
    {
        1.main函数如何被调用的
        2.命令行参数如何传送给执行程序
        3.典型的存储器布局
        4.如何分配另外的存储空间
        5.进程如何使用环境变量
        6.不同的进程终止方式
    }

    7.2 main函数如何被调用的
    {
        当内核执行程序时，在调用main前会先调用一个特殊的启动例程。
        可执行程序文件将此启动例程指定为程序的起始地址。
        形式一般为：exit(main(argc,argv));
        内核使程序执行的唯一方法是调用一个exec函数，从而调用启动例程，然后调用main函数。
    }

    7.3 命令行参数如何传送给执行程序
    {
        当执行一个程序的时候，调用exec的进程可将命令行参数传递给新程序。

        在deal_cmd.c展示了如何读命令、确定命令并调用相应的函数处理每一条命令。
        deal_cmd.c中用到了setjmp和longjmp函数。 
        当longjmp返回到setjmp处的时候，自动变量和寄存器变量的值不一定能恢复到以前调用setjmp的值。如果现在有一个自动变量，可以将其定义为volatile属性，让其无法回滚。全局变量和静态变量也无法回滚。
        下面用一个例程testjmp.c说明这个情况。
        注意，当声明自动变量的函数返回后，不能再引用这些自动变量。
        下面用一个例程open_data.c说明这个情况。
    }

    7.4 典型的存储器布局
    {
        从低地址到高地址，分别是正文段(由CPU执行的机器指令部分)、初始化的数据、未初始化的数据(bss段)、堆、栈、命令行参数和环境变量。
    }

    7.5 如何分配另外的存储空间
    {
        有三个用于存储空间动态分配的函数
        {
            1.void* malloc(size_t size);
            2.void* calloc(size_t nobj, size_t size);
            3.void* realloc(void* ptr, size_t newsize);
        }

        有一个用于释放ptr指向的存储空间
        {
            void free(void* ptr);
        }

        这些分配例程通常用sbrk(2)系统调用实现。
    }

    7.6 进程如何使用环境变量
    {
        每个程序都会接受到一张环境表。
        环境表是一个字符指针数组，其中每个指针都包含了一个以NULL结束的C字符串地址。
        全局变量environ则包含了该指针数组的地址: extern char **environ;
        通常使用getenv和putenv函数来访问特定的环境变量。
        当我们想要查看整个环境的时候，我们必须使用environ指针。
    }

    7.7 不同的进程终止方式
    {
        1.exit函数
        {
            void exit(int status);
            void _Exit(int status);
            void _exit(int status);
            
            以上三个函数用于正常终止一个程序，_exit和_Exit立即进入内核，而exit会先执行一些清理处理
        }

        2.atexit函数
        {
            int atexit(void (*func)(void));

            atexit函数用于登记终止处理程序。
            这些终止处理程序会被exit自动调用。exit调用这些函数的顺序与它们登记时候的顺序相反。
            在use_atexit.c中展示了如何使用atexit函数。
        }
    }
}