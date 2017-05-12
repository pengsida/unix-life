# linux内核下读写文件
## 核心代码：
首先开门见山亮代码：
```c
int read_file_init(void)
{
    struct file* file_ptr;
    mm_segment_t fs = get_fs();
    loff_t pos = 0;
    char buf[2] = {0, 0};
    char result[100] = {0};
    ssize_t count = 1;
    
    printk(KERN_ALERT "READ_FILE path: %s\n", path);
    
    printk(KERN_ALERT "READ_FILE: read_file_init\n");
    
    file_ptr = filp_open("/home/pengsida/Makefile", O_RDONLY, 0644);

    if (IS_ERR(file_ptr))
    {
        printk(KERN_ALERT "READ_FILE: open file fail\n");
        return -EFAULT;
    }
    set_fs(KERNEL_DS);
    
    while (1)
    {
        
        if(vfs_read(file_ptr, buf, count, &pos) <= 0)
        {
            printk(KERN_ALERT "READ_FILE: reach the end of file\n");
            break;
        }
        
        if (buf[0] == 'o')
        {
            if(get_specific_line(result, file_ptr, &pos) == 0)
            {
                squeeze(result);
                get_name(result);
                printk(KERN_ALERT "READ_FILE right: %s\n", result);
            }
        }
    }
    
    filp_close(file_ptr, NULL);
    set_fs(fs);
    return 0;
}
```

### 代码解析

主要代码流程为：
```c
struct file* file_ptr = filp_open("/home/pengsida/Makefile", O_RDONLY, 0644);
char buf[2] = {0};
size_t count = 1; // 每次读一个字节
loff_t pos = 0;
mm_segment_t fs = get_fs();

set_fs(KERNEL_DS);
while(1)
{
    if(vfs_write(file_ptr, buf, count, &pos) <= 0)
        break;
}
set_fs(fs);
filp_close(file_ptr, NULL);
```

上面的代码很简短，思路也很清晰，就是：

1. 使用filp_open()打开文件，原型函数如下：

        struct file *filp_open(const char *filename,int flags, int mode)

2. 使用get_fs()获得当前地址限制值：

        #define get_fs()	(current_thread_info()->addr_limit)

3. 将可访问地址扩大到内核空间：

        set_fs(KERNEL_DS);

4. 使用vfs_write()读取文件

5. 还原可访问地址，并关闭文件。

### 为什么要将可访问地址扩大到内核空间？

首先看vfs_read()的原型：

    ssize_t vfs_read(struct file *file,char __user *buf, size_t count, loff_t *pos)

这里buf指针指向用户空间，但我们这里其实是处于内核空间，接着看vfs_read函数的实现就知道了：
```c
ssize_t vfs_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
	...
    // 检查buf指针是否超出地址范围
	if (unlikely(!access_ok(VERIFY_WRITE, buf, count)))
		return -EFAULT;
    ...
}

#define access_ok(type, addr, size)				\
({								\
	__chk_user_ptr(addr);					\
	__access_ok(((unsigned long)(addr)), (size), get_fs());	\
})

/*
 * Is a address valid? This does a straightforward calculation rather
 * than tests.
 *
 * Address valid if:
 *  - "addr" doesn't have any high-bits set
 *  - AND "size" doesn't have any high-bits set
 *  - AND "addr+size" doesn't have any high-bits set
 *  - OR we are in kernel mode.
 */
#define __access_ok(addr, size, segment) \
	(((segment).seg & (addr | size | (addr+size))) == 0)
```

上面这段代码看起来可能有点懵，我也不是完全理解，但从英文注释中我们可以得到下述结论：

1. 如果get_fs()不为KERNEL_DS，如果指针addr处于内核空间，或者size处于内核空间，或者addr+size处于内核空间，就肯定报错。
2. 如果get_fs()为KERNEL_DS，就一定是access_ok。

这样一来二去，我们就知道了，现在我们处于内核空间，所以buf指针肯定是内核地址。
可是vfs_read()函数又是用户空间的，所以它的可访问地址是USER_DS。如果不把可访问地址设置成KERNEL_DS，
那肯定是要报错的。KERNEL_DS和USER_DS的宏定义如下：

    #define KERNEL_DS	((mm_segment_t) { 0UL })
    #define USER_DS		((mm_segment_t) { -0x40000000000UL })


## linux内核读写文件的实例

现在我想在linux内核下读取一个Makefile文件中的一行文字，Makefile内容如下：
```make
obj-m := scull_psd.o
scull_psd-objs := scull.o

KERNEL_DIR := /lib/modules/$(shell uname -r)/build

# 决定是否输出debug信息
IS_PRINT_DEBUG = y
ifeq ($(IS_PRINT_DEBUG), y)
    EXTRA_CFLAGS += -O -g -DPRINT_DEBUG
else
    EXTRA_CFLAGS += -O2
endif

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

clean:
rm -f $(shell ls | grep ".*mod\.c")
rm -f $(shell ls | egrep -v "(.*\.c|.*\.h|Makefile|.*\.sh)")
```

我现在想获取“obj-m := scull_psd.o”中“scull_psd”这个字段，实现代码如下：

```c
//
//  read_file.c
//  ldd
//
//  Created by pengsida on 2017/5/9.
//  Copyright © 2017年 pengsida. All rights reserved.
//

#include <linux/module.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/limits.h>
#include <linux/slab.h>
#include <asm/current.h>

static int get_specific_line(char* result, struct file* file_ptr, loff_t* pos)
{
    ssize_t count = 1;
    char buf[2] = {0};
    const char OBJ_M[] = "bj-m";
    int index = 0;
    memset(result, 0, 100);
    
    while (1)
    {
        if (vfs_read(file_ptr, buf, count, pos) <= 0)
        {
            printk(KERN_ALERT "READ_FILE: reach the end of file\n");
            break;
        }
        
        if (index < 4 && buf[0] != OBJ_M[index])
            break;
        
        if (buf[0] == '\n')
            return 0;
        
        result[index] = buf[0];
        
        index++;
    }
    
    return 1;
}

static void copy(char* from, char* to)
{
    while (*from && *to)
    {
        *to = *from;
        from++;
        to++;
    }
    *to='\0';
}

static void squeeze(char* result)
{
    int index = 0;

    while (*result)
    {
        index = 0;
        while(*(result+index) == ' ')
            index++;

        if(index)
            copy(result+index, result);
        
        result++;
    }
}

static void get_name(char* result)
{
    char* start = result;
    
    while (*result)
    {
        if (*result == '=')
        {
            copy(result+1, start);
            result = start;
            continue;
        }
        
        if (*result == '.')
        {
            *result = '\0';
            break;
        }
        
        result++;
    }
}

int read_file_init(void)
{
    struct file* file_ptr;
    mm_segment_t fs = get_fs();
    loff_t pos = 0;
    char buf[2] = {0, 0};
    char result[100] = {0};
    ssize_t count = 1;
    
    printk(KERN_ALERT "READ_FILE path: %s\n", path);
    
    printk(KERN_ALERT "READ_FILE: read_file_init\n");
    
    file_ptr = filp_open("/home/pengsida/Makefile", O_RDONLY, 0644);
    if (IS_ERR(file_ptr))
    {
        printk(KERN_ALERT "READ_FILE: open file fail\n");
        return -EFAULT;
    }
    set_fs(KERNEL_DS);
    
    while (1)
    {
        
        if(vfs_read(file_ptr, buf, count, &pos) <= 0)
        {
            printk(KERN_ALERT "READ_FILE: reach the end of file\n");
            break;
        }
        
        if (buf[0] == 'o')
        {
            if(get_specific_line(result, file_ptr, &pos) == 0)
            {
                squeeze(result);
                get_name(result);
                printk(KERN_ALERT "READ_FILE right: %s\n", result);
            }
        }
    }
    
    filp_close(file_ptr, NULL);
    set_fs(fs);
    return 0;
}

void read_file_exit(void)
{
    printk(KERN_ALERT "READ_FILE: read_file_exit\n");
}

module_init(read_file_init);
module_exit(read_file_exit);
```