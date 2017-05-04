//
//  scull.c
//  ldd
//
//  Created by pengsida on 2017/4/29.
//  Copyright © 2017年 pengsida. All rights reserved.
//

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/semaphore.h>
#include <linux/moduleparam.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>
MODULE_LICENSE("Dual BSD/GPL");

const int scull_nr_device = 4;
static int device_major_num = 0; // 主设备号
static int device_minor_num = 0; // 次设备号起始处
static int quantum_num = 5;
static int quantum_size = 6;

module_param(device_major_num, int, S_IRUGO);
module_param(quantum_num, int, S_IRUGO);
module_param(quantum_size, int, S_IRUGO);

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

struct scull_dev* scull_devices;

struct file_operations scull_ops = {
    .owner = THIS_MODULE,
};

/*
 1. 获得指向量子集链表的头部
 2. 随着链表释放每个量子集的空间
 */

void free_scull_device(struct scull_dev* scull_device)
{
    struct scull_qset* qset = scull_device->qset_list;
    struct scull_qset* temp = NULL;
    int i;
    
    while (qset)
    {
        if (qset->quantnum_array)
        {
            for (i = 0; i < scull_device->quantum_num; i++)
            {
                kfree(qset->quantnum_array[i]);
                qset->quantnum_array[i] = NULL;
            }
            kfree(qset->quantnum_array);
            qset->quantnum_array = NULL;
        }
        temp = qset->next;
        kfree(qset);
        qset = temp;
    }
    scull_device->total_size = 0;
    scull_device->qset_list = NULL;
}

/*
 1. 从系统中移除字符设备
 2. 释放字符设备的内存
 3. 释放scull_devices结构体
 4. 释放设备的主设备号：如果device_major_num为0，说明分配主设备号不曾成功
 */

static void scull_exit(void)
{
    dev_t device_num = MKDEV(device_major_num, device_minor_num);
    int i;
    
    if(scull_devices)
    {
        for(i = 0; i < scull_nr_device; i++)
        {
            cdev_del(&(scull_devices[i].dev));
            free_scull_device(scull_devices+i);
        }
        
        kfree(scull_devices);
        scull_devices = NULL;
    }
    
    if(device_major_num)
        unregister_chrdev_region(device_num, scull_nr_device);
    
    printk(KERN_ALERT "SCULL: see you lala\n");
}

/*
 1. 首先分配设备号，如果失败，就将device_major_num设为0，并跳转到fail处
 2. 初始化scull_nr_device这个数据结构: 分配内存；将它的值填充为0；初始化各成员值
 3. 初始化cdev代表字符设备的数据结构: 调用初始化函数；初始化各成员值
 4. 向系统注册字符设备
 */

static int scull_init(void)
{
    dev_t device_num;
    int i;
    
    if (device_major_num)
    {
        device_num = MKDEV(device_major_num, device_minor_num);
        if(register_chrdev_region(device_num, scull_nr_device, "scull"))
        {
            device_major_num = 0;
            goto fail;
        }
    }
    else
    {
        if (alloc_chrdev_region(&device_num, device_minor_num, scull_nr_device, "scull"))
        {
            device_major_num = 0;
            goto fail;
        }
        device_major_num = MAJOR(device_num);
    }
    
    printk(KERN_ALERT "SCULL: successfully allocate device num\n");
    
    scull_devices = kmalloc(scull_nr_device * sizeof(struct scull_dev), GFP_KERNEL);
    if(!scull_devices)
        goto fail;
    
    memset(scull_devices, 0, scull_nr_device * sizeof(struct scull_dev));
    
    for (i = 0; i < scull_nr_device; i++)
    {
        scull_devices[i].total_size = 0;
        scull_devices[i].quantum_num = quantum_num;
        scull_devices[i].quantum_size = quantum_size;
        scull_devices[i].qset_list = NULL;
        sema_init(&scull_devices[i].sem, 1);
        
        cdev_init(&(scull_devices[i].dev), &scull_ops);
        scull_devices[i].dev.ops = &scull_ops;
        scull_devices[i].dev.owner = THIS_MODULE;
        
        device_num = MKDEV(device_major_num, device_minor_num+i);
        if(cdev_add(&scull_devices[i].dev, device_num, 1))
            goto fail;
        printk(KERN_ALERT "SCULL: register %d\n", i);
    }
    
    printk(KERN_ALERT "SCULL: successfully register device\n");
    
    return 0;
    
fail:
    printk(KERN_ALERT "SCULL: something wrong\n");
    scull_exit();
    return -EFAULT;
}

module_init(scull_init);
module_exit(scull_exit);
