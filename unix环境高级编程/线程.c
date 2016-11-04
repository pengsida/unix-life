第十一章 线程

11.0 本章学习的东西
{
    1.了解如何使用多个线程在单进程环境中执行多个任务
    2.同步机制
}

11.3 线程标识
{
    进程ID在整个系统中是唯一的，但线程ID不同，线程ID只在它所属的进程环境中有效。
    线程ID用pthread_t来表示，而pthread_t是一个结构，所以需要相应的函数对两个线程ID进行比较。
    int pthread_equal(pthread_t tid1, pthread_t tids);
}