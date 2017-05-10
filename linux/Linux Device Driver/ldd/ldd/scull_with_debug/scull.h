//
//  scull.h
//  ldd
//
//  Created by pengsida on 2017/5/8.
//  Copyright © 2017年 pengsida. All rights reserved.
//

#ifndef scull_h
#define scull_h

#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/types.h>
/*
 设置是否输出DEBUG消息
*/
#undef SCULL_PRINT_DEBUG
#ifdef PRINT_DEBUG
#define SCULL_PRINT_DEBUG(fmt, args...) printk(KERN_ALERT, fmt, ##args)
#else
#define SCULL_PRINT_DEBUG(fmt, args...)
#endif

struct scull_qset
{
    void** quantnum_array;
    struct scull_qset* next;
};

struct scull_dev
{
    int total_size; // 表示设备文件的总大小
    int quantum_num; // 量子集中的量子个数
    int quantum_size; // 一个量子的大小
    
    struct scull_qset* qset_list; // 指向量子集链表的指针
    
    struct semaphore sem; // 互斥锁信号量
    struct cdev dev; // 用于代表一个字符设备
};

int scull_open(struct inode *, struct file *);
int scull_release(struct inode *, struct file *);
ssize_t scull_read(struct file *, char __user *, size_t, loff_t *);
ssize_t scull_write(struct file *, const char __user *, size_t, loff_t *);

#endif /* scull_h */
