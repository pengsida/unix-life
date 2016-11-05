#include <stdlib.h>
#include <pthread.h>

#define NHASH 29
#define HASH(fp) (((unsigned long) fp) % NHASH)

struct foo *fh[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER; // 用于守护hash表中的元素

struct foo
{
    int f_count;
    pthread_mutex_t f_lock;
    struct foo * f_next;
    int f_id;
};

struct foo * foo_alloc(void)
{
    struct foo *fp;
    int idx;

    if((fp = malloc(sizeof(struct foo))) != NULL)
    {
        fp->f_count = 1;
        if(pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return NULL;
        }
        idx = HASH(fp);
        pthread_mutex_lock(&hashlock); // 锁住散列列表锁，防止其他线程访问散列
        fp->f_next = fh[idx]; // 将新的结构添加进散列表
        fh[idx] = fp->f_next;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
        //...
        pthread_mutex_unlock(&fp->f_lock);
    }
    return fp;
}

void foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

// 寻找散列表中的元素
struct foo* foo_find(int id)
{
    struct foo *fp;
    int idx;
    idx = HASH(fp);
    pthread_mutex_lock(&hashlock);
    for(fp = fh[idx]; fp != NULL; fp = fp->f_next)
    {
        if(fp->f_id == id)
        {
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void foo_rele(struct foo *fp)
{
    struct foo *tfp;
    int idx;
    pthread_mutex_lock(&fp->f_lock);
    if(fp->f_count == 1) // 只要有if就会有竞争条件
    {
        // 如果要从散列列表中删除这个结构，就要先对这个结构互斥量进行解锁，才可以获取散列列表锁，然后重新获取结构互斥量。
        // 如果直接去尝试获取列表锁，如果此时另外一个线程持有hashlock，并且尝试获得该foo的锁，那么就造成了死锁。
        // 这里解锁了foo里面的元素，所以这两行代码之间，f_count可能被修改。
        pthread_mutex_unlock(&fp->f_lock); 
        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);

        // 从上一次获得结构互斥量以来可能处于被阻塞状态，所以需要重新检查条件。
        // 如果其他线程在我们为满足锁顺序而阻塞时发现了这个结构并对其引用计数加一，那么只要简单地对引用计数减一，对所有的东西解锁然后返回。
        if(fp->f_count != 1)
        {
            fp->f_count--;
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return;
        }
        idx = HASH[fp];
        tfp = fh[idx];
        if(tfp == fp)
            fh[idx] = fp->f_next;
        else
        {
            while(tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
    {
        fp->f_count--;
        pthread_mutex_unlock(&fp->f_lock);
    }
}


// 上述加解锁比较复杂。可以用散列列表锁来保护结构引用计数，而结构互斥量用于保护foo结构中的其他任何东西。

#include <stdlib.h>
#include <pthread.h>

#define NHASH 29
#define HASH(fp) (((unsigned long) fp) % NHASH)

struct foo *fh[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo
{
    int f_count;
    pthread_mutex_t f_lock;
    struct foo *f_next;
    int f_id;
};

struct foo* foo_alloc(void)
{
    struct foo *fp;
    int idx;

    if((fp = malloc(sizeof(struct foo))) != NULL)
    {
        fp->f_count = 1;
        if(pthread_mutex_init(&fp->f_lock, NULL) != 0)
        {
            free(fp);
            return NULL;
        }
        idx = HASH(fp);
        pthread_mutex_lock(&hashlock);
        fp->f_next = fh[idx];
        fh[idx] = fp->f_next;
        pthread_mutex_lock(&fp->f_lock); // 必须在锁住散列表时锁住foo结构。因此此时foo结构还未初始化，如果解锁散列表，在锁上foo结构之前，其他线程可能通过散列表访问到foo结构。
        pthread_mutex_unlock(&hashlock);
        //...
        pthread_mutex_unlock(&fp->f_lock);
    }
    return(fp);
}

void foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&hashlock);
    fp->f_count++;
    pthread_mutex_unlock(&hashlock);
}

struct foo * foo_find(int id)
{
    struct foo *fp;
    int idx;
    idx = HASH(fp);
    pthread_mutex_lock(&hashlock);
    for(fp = fh[idx]; fp != NULL; fp = fp->f_next)
    {
        if(fp->f_id == id)
        {
            fp->f_count++;
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return fp;
}

void foo_rele(struct foo *fp)
{
    struct foo *tfp;
    int idx;

    pthread_mutex_lock(&hashlock);
    if(--fp->f_count == 0)
    {
        idx = HASH(fp);
        tfp = fh[idx];
        if(tfp == fp)
            fh[idx] = fp->f_next;
        else
        {
            while(tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        // fp已经被移出散列表了，其他线程无法通过散列表访问到该结构，所以这里可以解开散列表的锁。
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }
    else
        pthread_mutex_unlock(&hashlock);
}