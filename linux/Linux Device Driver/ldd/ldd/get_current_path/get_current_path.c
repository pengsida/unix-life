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
    kfree(current_path);
    
    return 0;
}

static void get_current_path_exit(void)
{
    printk(KERN_ALERT "GET_CURRENT_PATH: exit\n");
}

module_init(get_current_path_init);
module_exit(get_current_path_exit);
