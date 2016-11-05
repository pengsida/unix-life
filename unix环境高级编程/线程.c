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

    线程可以通过调用pthread_t pthread_self()函数获得自身线程ID。

    创建线程的函数
    int pthread_create(pthread_t *restrict tidp, const pthread_attr_t *restrict attr, void *(*start_rtn)(void), void *restrict arg);

    线程终止方式：从启动例程中退出，被同一进程中的其他线程取消，调用pthread_exit()。
    void pthread_exit(void *rval_ptr);

    int pthread_join(pthread_t thread, void **rval_ptr);

    线程可以通过调用pthread_t cancel(pthread_t tid)来请求取消同一进程中的其他进程。
    线程清理函数pthread_cleanup_push(void (*rtn)(void *), void *arg)和void pthread_cleanup_pop(int execute);
    
    在默认情况下，线程的终止状态会保存到对该线程调用pthread_join。
    当线程处于分离状态时，线程的底层存储资源可以在线程终止时立即被收回。
    使用int pthread_detach(pthread_t tid)让线程进入分离状态。
}

11.6 线程同步
{
    1.互斥量
    {
        互斥量本质上说是一把锁，在访问共享资源之前，对互斥量进行加锁，在访问完成后释放互斥量上的锁。
        对互斥量进行加锁以后，任何其他试图对互斥量进行加锁的线程将会被阻塞直到当前线程释放该互斥锁。
        在使用互斥量之前需要进行初始化，使用这个函数：int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr)
        在释放内存前，需要调用这个函数：int pthread_mutex_destroy(pthread_mutex_t *mutex);
        对互斥量进行加锁：int pthread_mutex_lock(pthread_mutex_t *mutex)
        如果线程希望尝试加锁时不被阻塞，应该使用这个函数：int pthread_mutex_trylock(pthread_mutex_t *mutex)
        对互斥量进行解锁，需要调用int pthread_mutex_unlock(pthread_mutex_t *mutex)
    }

    2.避免死锁
    {
        可以通过小心地控制互斥量加锁的顺序来避免死锁的发生。

        如果锁的粒度太粗，就会出现许多线程阻塞等待相同的锁，并发性就微乎其微。
        如果锁的粒度太细，那么过多的锁开销会使系统性能收到影响，而且代码将变得相当复杂。                  
    }

    3.读写锁
    {
        读写锁有三种状态：读模式下加锁状态，写模式下加锁状态和不加锁状态。
        读写锁也叫做共享-独占锁，当读写锁以读模式锁住时，它是以共享模式锁住的。当它以写模式锁住时，它是以独占模式锁住的。
        初始化读写锁时，使用这个函数：int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t attr);
        销毁读写锁时，使用这个函数：int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
        要在读模式下锁定读写锁，使用这个函数：int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
        要在写模式下锁定读写锁，使用这个函数：int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
        对读写锁进行解锁，需要使用这个函数：int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
        如果希望进行加锁而不被阻塞，使用这两个函数：int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)和int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
    }

    4.条件变量
    {
        条件变量：线程可用的另一种同步机制。
        条件变量与互斥量一起使用时，允许线程以无竞争的方式等待特定的条件发生。
        初始化条件变量时，使用这个函数：int pthread_cond_init(pthread_cond_t *restrict cond, pthread_condattr_t *restrict attr)
        销毁条件变量时，使用这个函数：int pthread_cond_destroy(pthread_cond_t *cond)
        使用条件变量等待条件变为真，有这个函数：int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex)
        可以给定时间等待条件变为真，有这个函数：int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict timeout)
        上述函数的第三个参数类型是timespec，是一个结构体
        {
            struct timespec
            {
                time_t tv_sec;
                long tv_nsec;
            };
            时间值是一个绝对数而不是相对数。
        }
        条件变量的工作机制：调用者把锁住的互斥量传给函数pthread_cond_wait()，让互斥量对条件进行保护，使得条件检验时条件不发生变化，函数把调用线程放到等待条件的线程列表上，然后对互斥量解锁。
        如果条件满足了，使用这个函数将等待线程唤醒：int pthread_cond_signal(pthread_cond_t *cond)
        如果条件满足了，使用这个函数将所有等待线程唤醒：int pthread_cond_broadcast(pthread_cond_t *cond)
    }
}