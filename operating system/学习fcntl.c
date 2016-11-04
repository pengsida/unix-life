// 先是fcntl.h头文件的代码

#include <sys/types.h>

#define O_ACCMODE 00003
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02
#define O_CREAT 00100
#define O_EXCL 00200
#define O_NOCTTY 00400
#define O_TRUNC 01000
#define O_APPEND 02000
#define O_NONBLOCK 04000
#define O_NDELAY O_NONBLOCK

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7

#define FD_CLOEXEC 1

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

struct flock
{
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
};

extern int creat(const char * filename, mode_t mode);
extern int fcntl(int fildes, int cmd, ...);
extern int open(const char * filename, int flags, ...); // C语言允许行参部分可以使用省略号代替。省略号告诉编译器，在函数调用时不检查行参是否与实参类型相同，也不检查参数个数。



//////////////////////////////////////////////////

#include <stirng.h>
#include <errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <fcntl.h>
#include <sys/stat.h>

extern int sys_close(int fd);

// fcntl实现了fcntl()和dup()、dup2()函数。
// fd是被复制的文件的文件描述符，arg是新句柄
// 文件句柄就是进程文件结构指针数组项索引号
static int dupfd(unsigned int fd, unsigned int arg)
{
    if(fd >= NR_OPEN || !current->filp[fd])
        return -EBADF;
    if(arg >= NR_OPEN)
        return -EINVAL;
    // 寻找大等于arg的索引号，且该索引号指向的进程文件结构指针数组中的项还未使用
    while(arg < NR_OPEN)
        if(current->filp[arg])
            arg++;
        else
            break;
    if(arg >= NR_OPEN)
        return -EMFILE;
    // 执行该语句使得close_exec_on被清除，于是之后运行exec()函数时不会关闭用dup()建立的文件句柄。
    current->close_on_exec &= ~(1<<arg);
    (current->filp[arg] = current->filp[fd])->f_count++;
    return arg;
}

// 函数dup()和dup2()所返回的新文件句柄与被复制原句柄将共同使用同一个文件表项。
int sys_dup2(unsigned int oldfd, unsigned int newfd)
{
    sys_close(newfd);
    return dupfd(oldfd, newfd);
}

int sys_dup(unsigned int fildes)
{
    return dupfd(fildes, 0);
}

// 用于修改已打开的状态或复制句柄操作。
int sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg)
{
    struct file * filp;
    if(fd >= NR_OPEN || !(filp = current->filp[fd]))
        return -EBADF;
    switch(cmd)
    {
        case F_DUPFD:
            return dupfd(fd, arg);
        case F_GETFD;
            return (current->close_on_exec>>fd) & 1;
        case F_SETFD:
            if(arg&1)
                current->close_on_exec |= (1<<fd);
            else
                current->close_on_exec &= ~(1<<fd);
            return 0;
        case F_GETFL:
            return filp->f_flags;
        case F_SETFL:
            filp->f_flags &= ~(O_APPEND | O_NONBLOCK);
            filp->f_flags |= arg & (O_APPEND | O_NONBLOCK);
            return 0;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
            return -1;
        default:
            return -1;
    }
}