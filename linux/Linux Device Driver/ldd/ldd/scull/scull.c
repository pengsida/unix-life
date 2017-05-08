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
#include <asm-generic/uaccess.h>
#include <asm/fcntl.h>
#include <linux/module.h>
#include <linux/init.h>
#include "scull.h"
MODULE_LICENSE("Dual BSD/GPL");

const int scull_nr_device = 4;
static int device_major_num = 0; // 主设备号
static int quantum_num = 128;
static int quantum_size = 64;

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

struct file_operations scull_ops = {
    .owner = THIS_MODULE,
    .open = scull_open,
    .release = scull_release,
    .read = scull_read,
    .write = scull_write,
};

////////////////////////////////////////////////////
//                                                //
//                  辅助函数                       //
//                                                //
////////////////////////////////////////////////////

/*
 1. 获得指向量子集链表的头部
 2. 随着链表释放每个量子集的空间
 */

static void free_scull_device(struct scull_dev* scull_device)
{
    struct scull_qset* qset = scull_device->qset_list;
    struct scull_qset* temp = NULL;
    int i;
    int j = 0;
    
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
        // printk(KERN_ALERT "SCULL in free: %d\n", j++);
    }
    
    scull_device->total_size = 0;
    scull_device->qset_list = NULL;
}

struct scull_qset* alloc_qset(int quantum_num, int quantum_size)
{
    struct scull_qset* qset = (struct scull_qset*)kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
    int i;
    
    if (!qset)
        goto fail;
    
    qset->next = NULL;
    qset->quantnum_array = kmalloc(quantum_num * sizeof(char*), GFP_KERNEL);
    
    if (!qset->quantnum_array)
        goto fail;
    
    for (i = 0; i < quantum_num; i++)
    {
        qset->quantnum_array[i] = kmalloc(quantum_size * sizeof(char), GFP_KERNEL);
        if (!qset->quantnum_array[i])
            goto fail;
        memset(qset->quantnum_array[i], 0, quantum_size * sizeof(char));
    }
    
    return qset;
    
fail:
    printk(KERN_ALERT "SCULL: memory is short in alloc_qset\n");
    
    for (i = 0; i < quantum_num; i++)
    {
        if (qset->quantnum_array[i])
        {
            kfree(qset->quantnum_array[i]);
            qset->quantnum_array[i] = NULL;
        }
        else
            break;
    }
    
    if (qset->quantnum_array)
    {
        kfree(qset->quantnum_array);
        qset->quantnum_array = NULL;
    }
    
    if (qset)
    {
        kfree(qset);
        qset = NULL;
    }
    return NULL;
}

struct scull_qset* get_qset(struct scull_dev* scull_device, int qset_index)
{
    struct scull_qset* qset_list = scull_device->qset_list;
    struct scull_qset* temp;
    
    if (!qset_list)
        scull_device->qset_list = qset_list = alloc_qset(scull_device->quantum_num, scull_device->quantum_size);
    
    while (qset_index)
    {
        if (qset_list->next)
            qset_list = qset_list->next;
        else
        {
            temp = alloc_qset(scull_device->quantum_num, scull_device->quantum_size);
            qset_list->next = temp;
            qset_list = temp;
        }
        qset_index--;
    }
    
    return qset_list;
}

////////////////////////////////////////////////////
//                                                //
//                  设备操作                       //
//                                                //
////////////////////////////////////////////////////

/*
 1. 从file_node中获得cdev结构体，从而得到scull_dev结构体
 2. 将file_ptr中的private_data成员设为scull_dev结构体
 3. 如果文件打开模式为写模式，则将设备文件清空
*/
int scull_open(struct inode* file_node, struct file* file_ptr)
{
    struct cdev* char_device = file_node->i_cdev;
    struct scull_dev* scull_device = container_of(char_device, struct scull_dev, dev);
    
    file_ptr->private_data = scull_device;
    
    if((file_ptr->f_flags & O_ACCMODE) == O_WRONLY)
        free_scull_device(scull_device);
    
    return 0;
}

int scull_release(struct inode* file_node, struct file* file_ptr)
{
    return 0;
}

/*
 1. 首先通过file_ptr获得指向内存区域的scull_dev结构体
 2. 通过f_pos获得第几个量子集，第几个量子，量子中的第几个位置
 3. 通过copy_to_user()将内存区域的内容读取到用户空间
*/
ssize_t scull_read(struct file* file_ptr, char __user* buf, size_t count, loff_t* f_pos)
{
    struct scull_dev* scull_device = (struct scull_dev*)file_ptr->private_data;
    struct scull_qset* qset = NULL;
    int qset_index = ((long)(*f_pos)) / (scull_device->quantum_num * scull_device->quantum_size);
    int memory_rest = ((long)(*f_pos)) % (scull_device->quantum_num * scull_device->quantum_size);
    int quantum_index = memory_rest / scull_device->quantum_size;
    int quantum_offset = memory_rest % scull_device->quantum_size;
    int retval = 0;
    
    // printk(KERN_ALERT "SCULL in read: start\n");
    
    if(down_interruptible(&scull_device->sem))
    {
        printk(KERN_ALERT "SCULL in scull_read: something wrong happened to down interrupt\n");
        retval = -ERESTARTSYS;
        goto finish;
    }
    
    if (*f_pos > scull_device->total_size)
    {
        printk(KERN_ALERT "SCULL in scull_read: f_pos is bigger than total_size\n");
        goto finish;
    }
    
    if (*f_pos + count > scull_device->total_size) {
        count = ((long)*f_pos) - scull_device->total_size;
    }
    
    if(count > scull_device->quantum_size - quantum_offset)
        count = scull_device->quantum_size - quantum_offset;
    
    qset = get_qset(scull_device, qset_index);
    
    if (copy_to_user(buf, qset->quantnum_array[quantum_index]+quantum_offset, count))
    {
        printk(KERN_ALERT "SCULL in scull_read: copy_to_user fail\n");
        retval = -EFAULT;
        goto finish;
    }
    
    *f_pos += (loff_t)count;
    retval = (int)count;
    
    // printk(KERN_ALERT "SCULL in read: success\n");
    
finish:
    up(&scull_device->sem);
    return retval;
}

/*
 思路与scull_read()类似
*/
ssize_t scull_write(struct file* file_ptr, const char __user* buf, size_t count, loff_t* f_pos)
{
    struct scull_dev* scull_device = (struct scull_dev*)file_ptr->private_data;
    struct scull_qset* qset;
    int qset_index = ((long)*f_pos) / (scull_device->quantum_num * scull_device->quantum_size);
    int memory_rest = ((long)*f_pos) % (scull_device->quantum_num * scull_device->quantum_size);
    int quantum_index = memory_rest / scull_device->quantum_size;
    int quantum_offset = memory_rest % scull_device->quantum_size;
    int retval = 0;
    
    // printk(KERN_ALERT "SCULL in scull_write: start\n");
    
    if (down_interruptible(&scull_device->sem))
    {
        printk(KERN_ALERT "SCULL in scull_write: something wrong happened to down interrupt\n");
        retval = -ERESTARTSYS;
        goto finish;
    }
    
    if (count > scull_device->quantum_size - quantum_offset)
        count = scull_device->quantum_size - quantum_offset;
    
    qset = get_qset(scull_device, qset_index);
    
    if (copy_from_user(qset->quantnum_array[quantum_index]+quantum_offset, buf, count))
    {
        printk(KERN_ALERT "SCULL in scull_write: copy_from_user fail\n");
        retval = -EFAULT;
        goto finish;
    }
    
    // printk(KERN_ALERT "SCULL in scull_write: success\n");
    
    *f_pos += (loff_t)count;
    scull_device->total_size += count;
    retval = count;
    
finish:
    up(&scull_device->sem);
    return retval;
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
    struct scull_dev* scull_devices;
    dev_t device_num;
    int device_minor_num = 0; // 次设备号起始处
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
        // printk(KERN_ALERT "SCULL: %d, %d", MAJOR(device_num), MINOR(device_num));
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
