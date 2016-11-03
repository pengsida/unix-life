第十四章 高级I/O

14.0 本章学习内容
{
    1.非阻塞I/O
    2.记录锁
    3.系统V流机制
    5.I/O多路转接
}

14.1 非阻塞I/O
{
    低速系统调用：可能使进程永远阻塞的一类系统调用。

    指定非阻塞I/O的方法
    {
        1.调用open获得描述符，则可指定O_NONBLOCK标志。
        2.对于一个已经打开的描述符，可以调用fcntl，由该函数打开O_NONBLOCK文件状态标志。

        在test_io.c中给出了一个非阻塞I/O的实例。
    }
}

14.2 记录锁
{
    记录锁的功能：当一个进程正在读或修改文件的某个部分时，它可以阻止其他进程修改同一文件区。记录锁可以锁文件中的一个区域，也可以锁整个文件。

    fcntl记录锁
    {
        int fcntl(int fields, int cmd, struct flock *flockptr);
        fcntl函数的第二个参数有三种情况，分别是F_GETLK、F_SETLK和F_SETLKW。程序用F_GETLK测试是否建立一把锁，用F_SETLK和F_SETLKW企图建立一把锁。
        F_GETLK实际上很少使用，因为如果用F_GETLK获得锁的状态，再用F_SETLK，这两个不是原子操作，很可能被其他进程抢占。如此一来，不如直接用F_SETLK。
        fcntl函数的第三个参数是一个指向flock结构的指针。
        struct flock
        {
            short l_type;  // 锁的类型：共享读锁F_RDLCK、独占性写锁F_WRLCK或解锁一个区域F_UNLCK

            // l_start和l_whence决定加锁或解锁区域的起始字节偏移量。
            off_t l_start; // 表明了相对偏移量
            short l_whence; // 决定了相对偏移量的起点

            off_t l_len; // 区域的字节长度。
            pid_t l_pid; // 锁持有的进程的ID
        }
    }

    基本规则：多个进程在一个给定的字节上可以有一把共享的读锁，但是在一个给定字节上只能有一个进程读用的一把写锁。读锁是共享性的，写锁是独占性的。

    在test_lock.c中给出了关于锁的一些实例
}