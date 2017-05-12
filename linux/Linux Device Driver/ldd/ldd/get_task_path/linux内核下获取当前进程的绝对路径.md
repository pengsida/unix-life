# linux内核下获取当前进程的绝对路径
## 核心代码
首先开门见山贴代码：
```c
static void get_task_path(struct task_struct* task, char** task_path)
{
    struct vm_area_struct* vma;
    struct path file_path;
    size_t size = 512;
    char* get_path = kmalloc(size, GFP_KERNEL);
    memset(get_path, 0, size);
    
    task_lock(task);
    
    if(!task->mm || !task->mm->mmap)
        return;
    
    for (vma = task->mm->mmap; vma; vma = vma->vm_next)
    {
        if((vma->vm_flags & VM_EXEC) && vma->vm_file)
        {
            file_path = vma->vm_file->f_path;
            break;
        }
    }
    
    task_unlock(task);
    
    *task_path = d_path(&file_path, get_path, (int)size);
}
```

查看效果：
```bash
# 安装模块
pengsida@scholes:~/ldd/get_task_path$ sudo make install
[sudo] pengsida 的密码： 
insmod ./get_task_path.ko
# 卸载模块
pengsida@scholes:~/ldd/get_task_path$ sudo make uninstall 
rmmod get_task_path
# 查看效果
pengsida@scholes:~/ldd/get_task_path$ cat /var/log/syslog | grep "GET_TASK_PATH"
May 11 15:38:12 scholes kernel: [95620.958885] GET_TASK_PATH: init
May 11 15:38:12 scholes kernel: [95620.958888] GET_TASK_PATH: task_path is /bin/kmod
May 11 15:39:17 scholes kernel: [95686.053430] GET_TASK_PATH: exit
```

## 代码解析
这段代码思路很清晰，步骤如下：

1. 我们现在唯一拥有的是task这个task_struct数据结构，但我们可以通过它获得进程的虚拟内存空间(这个之后会介绍)：

		task->mm->mmap

2. 虚拟内存vm_area_struct是进程的地址空间，我们通过遍历它，可以找到进程的可执行文件所在的路径：
	```c
	for (vma = task->mm->mmap; vma; vma = vma->vm_next)
	{
		if((vma->vm_flags & VM_EXEC) && vma->vm_file)
		{
			file_path = vma->vm_file->f_path;
			break;
		}
	}
	```

3. 随后我们通过d_path函数获取相应的路径，d_path的原型函数如下：
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

## struct vm_area_struct的介绍

linux内核使用vm_area_struct结构来表示一个独立的虚拟内存区域，由于每个不同质的虚拟内存区域功能和内部机制都不同，因此一个进程使用多个vm_area_struct结构来分别表示不同类型的虚拟内存区域。各个vm_area_struct结构使用链表或者树形结构链接，方便进程快速访问，如下图所示：
<center><img src="https://github.com/wwtalwtaw/markdown-photos/raw/master/vm_area_struct.png"/></center>

假如该vm_area_struct描述的是一个文件映射的虚存空间，成员vm_file便指向被映射的文件的file结构，vm_pgoff是该虚存空间起始地址在vm_file文件里面的文件偏移，单位为物理页面。

## 完整代码
```c
//
//  get_task_path.c
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
#include <linux/path.h>
#include <linux/slab.h>

static void get_task_path(struct task_struct* task, char** task_path)
{
    struct vm_area_struct* vma;
    struct path file_path;
    size_t size = 512;
    char* get_path = kmalloc(size, GFP_KERNEL);
    memset(get_path, 0, size);
    
    task_lock(task);
    
    if(!task->mm || !task->mm->mmap)
        return;
    
    for (vma = task->mm->mmap; vma; vma = vma->vm_next)
    {
        if((vma->vm_flags & VM_EXEC) && vma->vm_file)
        {
            file_path = vma->vm_file->f_path;
            break;
        }
    }
    
    task_unlock(task);
    
    *task_path = d_path(&file_path, get_path, (int)size);
}

static int get_task_path_init(void)
{
    char* task_path = NULL;
    struct task_struct* task = current;
    
    printk(KERN_ALERT "GET_TASK_PATH: init\n");
    
    get_task_path(task, &task_path);
    
    printk(KERN_ALERT "GET_TASK_PATH: task_path is %s\n", task_path);
    
    return 0;
}

static void get_task_path_exit(void)
{
    printk(KERN_ALERT "GET_TASK_PATH: exit\n");
}

module_init(get_task_path_init);
module_exit(get_task_path_exit);

```

## Makefile文件
```
obj-m := get_task_path.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

modules:
    make -C $(KERNEL_DIR) M=$(shell pwd) modules

install:
    insmod ./get_task_path.ko

uninstall:
    rmmod get_task_path
```