# linux内核下获取当前路径
## 核心代码
首先开门见山贴代码：
```c
static void get_current_path(struct task_struct* task, char** current_path)
{
    struct path file_path;
    size_t size = 512;
    char* get_path = kmalloc(size, GFP_KERNEL);
    memset(get_path, 0, size);
    
    task_lock(task);
    
    file_path = task->fs->pwd;
    
    task_unlock(task);
    
    *current_path = d_path(&file_path, get_path, (int)size);
}
```

查看效果：
```bash
# 安装模块
pengsida@scholes:~/ldd/get_current_path$ sudo make install
insmod ./get_current_path.ko
# 卸载模块
pengsida@scholes:~/ldd/get_current_path$ sudo make uninstall
rmmod get_current_path
# 查看效果
pengsida@scholes:~/ldd/get_current_path$ cat /var/log/syslog | grep GET_CURRENT_PATH
May 11 16:39:17 scholes kernel: [99286.017373] GET_CURRENT_PATH: init
May 11 16:39:17 scholes kernel: [99286.017377] GET_CURRENT_PATH: current_path is /home/pengsida/ldd/get_current_path
May 11 16:39:23 scholes kernel: [99291.722779] GET_CURRENT_PATH: exit
```

## 代码解析
这段代码思路很清晰，步骤如下：

1. 我们现在唯一拥有的是task这个task_struct数据结构，我们可以通过它获得代表当前路径的task->fs->pwd。
2. 随后我们通过d_path函数获取相应的路径，d_path的原型函数如下：
	```c
	/**
	* d_path - return the path of a dentry
	* @path: path to report
	* @buf: buffer to return value in
	* @buflen: buffer length
	*
	* Convert a dentry into an ASCII path name. If the entry has been deleted
	* the string " (deleted)" is appended. Note that this is ambiguous.
	*
	* Returns a pointer into the buffer or an error code if the path was
	* too long. Note: Callers should use the returned pointer, not the passed
	* in buffer, to use the name! The implementation often starts at an offset
	* into the buffer, and may leave 0 bytes at the start.
	*
	* "buflen" should be positive.
	*/
	char *d_path(const struct path *path, char *buf, int buflen);
	```

## 完整代码
```c
//
//  get_current_path.c
//  ldd
//
//  Created by pengsida on 2017/5/11.
//  Copyright © 2017年 pengsida. All rights reserved.
//

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/path.h>
#include <linux/slab.h>

static void get_current_path(struct task_struct* task, char** current_path)
{
    struct path file_path;
    size_t size = 512;
    char* get_path = kmalloc(size, GFP_KERNEL);
    memset(get_path, 0, size);
    
    task_lock(task);
    
    file_path = task->fs->pwd;
    
    task_unlock(task);
    
    *current_path = d_path(&file_path, get_path, (int)size);
}

static int get_current_path_init(void)
{
    char* current_path = NULL;
    struct task_struct* task = current;
    
    printk(KERN_ALERT "GET_CURRENT_PATH: init\n");
    
    get_current_path(task, &current_path);
    
    printk(KERN_ALERT "GET_CURRENT_PATH: current_path is %s\n", current_path);
    
    return 0;
}

static void get_current_path_exit(void)
{
    printk(KERN_ALERT "GET_CURRENT_PATH: exit\n");
}

module_init(get_current_path_init);
module_exit(get_current_path_exit);
```

## Makefile
```make
obj-m := get_current_path.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

install:
    insmod ./get_current_path.ko

uninstall:
    rmmod get_current_path
```
