#include "apue.h"
#include <fcntl.h>

int lock_reg(int fd, int cmd, off_t offset, int whence, off_t len)
{
    struct flock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;
    return fcntl(fd, cmd, &lock);
}

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock = lock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;
    // F_GETLK：判断由flockptr所描述的锁是否被另外一把锁所排斥。如果不存在这种情况，就将l_type改为F_UNLCK。如果存在能阻塞当前进程的锁，其持有进程的ID就存放到l_pid中。
    if(fcntl(fd, F_GETLK, &lock) < 0)
        err_sys("fcntl error");
    // flockptr所描述的锁没有被文件上的另一把锁所排斥的情况
    if(lock.l_type == F_UNLCK)
        return 0;
    // 返回持有锁的进程
    return lock.l_pid;
}