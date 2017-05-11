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
    kfree(task_path);
    
    return 0;
}

static void get_task_path_exit(void)
{
    printk(KERN_ALERT "GET_TASK_PATH: exit\n");
}

module_init(get_task_path_init);
module_exit(get_task_path_exit);

