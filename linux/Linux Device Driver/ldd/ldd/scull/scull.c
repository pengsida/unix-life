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
#include <asm-generic/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>

struct scull_qset
{
    void **quantum_array;
    struct scull_qset *next;
};

struct scull_dev
{
    struct scull_qset *data; // 指向量子集
    int quantum_size;        // 当前量子的大小
    int qset_size;           // 当前数组的大小
    unsigned long size;      // 数据总量
    unsigned int access_key;
    struct semaphore sem;    // 互斥信号量
    struct cdev cdev;        // 字符设备结构
};

int scull_init(int scull_major, int scull_minor)
{
    dev_t dev;
    int result;
    int scull_nr_devs = 5;
    if( scull_major )
    {
        dev = MKDEV(scull_major, scull_minor); // 生成设备号，scull_major为主设备号，scull_minor为次设备号
        result = register_chrdev_region(&dev, scull_nr_devs, "scull"); // 静态分配设备号
    }
    else
    {
        result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
        scull_major = MAJOR(dev);
    }
    
    if( result < 0 )
    {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }
}

// 注册一个字符设备
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err;
    int devno = MKDEV(scull_major, scull_minor + index);
    cdev_init(&dev->cdev, &scull_fops); // 初始化cdev结构体和file_operations结构体
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &scull_fops;
    err = cdev_add(&dev->cdev, devno, 1); // 通知内核cdev结构体的信息
    if(err)
        printk(KERN_WARNING "Error %d adding scull%d", err, index);
}

// 释放整个数据区
int scull_trim(struct scull_dev* dev)
{
    struct scull_qset* next;
    struct scull_qset* qset;
    int qset = dev->qset;
    int i;
    for(qset = (scull_qset*)dev->data; qset; qset = next)
    {
        if(qset->quantum_array)
        {
            for(i = 0; i < qset; i++)
            {
                kfree(dptr->quantum_array[i]);
            }
            kfree(dptr->quantum_array);
            dptr->quantum_array = NULL;
        }
        next = qset->next;
        kfree(qset);
        qset = NULL;
    }
    dev->size = 0;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->data = NULL;
    return 0;
}

// 得到第n个scull_qset
// 如果没有，就一路给qset分配空间
struct scull_qset* get_qset(struct scull_dev* dev, int n)
{
    struct scull_qset* qset = dev->data;
    
    if(!qset)
    {
        qset = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
        if(qset == NULL)
            return NULL;
        memset(qset, 0, sizeof(struct scull_qset));
    }
    
    while(n--)
    {
        if(!qset->next)
        {
            qset->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
            if(qset->next == NULL)
                return NULL;
            memset(qset->next, 0, sizeof(struct scull_qset));
        }
        qset = qset->next;
    }
    
    return qset;
}

// 分配并填写置于filp->private_data里的数据结构
int scull_open(struct inode* node, struct file* filp)
{
    struct scull_dev* dev;
    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev;
    
    if((file->f_flags & O_ACCMODE) == O_WRONLY)
        scull_trim(dev);
    return 0;
}

int scull_release(struct inode* inode, struct file* filp)
{
    return 0;
}

/*
 输入参数为filp，目的地址buf，要读取的数据量count，初始位置f_pos
 1. 根据f_pos可以得到相应的量子集scull_qset，相应的量子，以及偏移量
 2. 加锁
 3. 检查f_pos是否大于dev的总大小，如果是，就直接返回
 4. 检查f_pos+count是否大于dev的总大小，如果是，就减小count
 5. 获取量子集
 6. 真正读取数据使用的是copy_to_user()函数
 需要注意的是，这个read一次最多处理一个量子
 */
ssize_t scull_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos)
{
    struct scull_dev* dev = filp->private_data;
    struct scull_qset* qset;
    int qset_index = (long)*f_pos / (dev->quantum_size * dev->qset_size);
    int rest = (long)*f_pos % (dev->quantum_size * dev->qset_size);
    int quantum_index = rest / dev->quantum_size;
    int quantum_offset = rest % dev->quantum_size;
    ssize_t retval = 0;
    
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;
    
    if(*f_pos >= dev->size)
        goto out;
    
    if(*f_pos + count > dev->size)
        count = dev->size - *f_pos;
    
    qset = get_qset(dev, qset_index);
    if(qset == NULL || !qset->data || !qset->data[quantum_index])
        goto out;
    
    if(count > dev->quantum_size - quantum_offset)
        count = dev->quantum_size - quantum_offset;
    
    if(copy_to_user(buf, qset->data[quantum_index]+quantum_offset, count))
    {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval = count;
    
out:
    up(&dev->sem);
    return retval;
}

/*
write一次只能处理一个量子
1. 根据输入参数得到内存区域的初始位置
    通过filp获得scull_dev结构体，也就有了量子集data，也有了量子集的大小
    根据f_pos可以得到初始位置在第几个量子集，也可以得到偏移位置rest
    根据rest可以得到初始位置在第几个量子，也可以得到偏移位置offset
2. 真正写入数据的函数是copy_from_user()
3. 更新一些数据
*/
ssize_t scull_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos)
{
    struct scull_dev* dev = filp->private_data;
    struct scull_qset* qset;
    int qset_index = (long)*f_pos / (dev->quantum_size * dev->qset_size);
    int rest = (long)*f_pos / (dev->quantum_size * dev->qset_size);
    int quantum_index = rest / (dev->quantum_size);
    int quantum_offset = rest % (dev->quantum_size);
    ssize_t retval = -ENOMEM;
    
    if(down_interruptible(&dev->sem))
        goto out;
    
    qset = get_qset(dev, qset_index);
    if (qset == NULL)
        goto out;
    
    if (!qset->quantum_array) {
        qset->quantum_array = kmalloc(dev->qset_size * sizeof(char*), GFP_KERNEL);
        if(!qset->quantum_array)
            goto out;
        memset(qset->quantum_array, 0, dev->qset_size * sizeof(char *));
    }
    
    if(!qset->quantum_array[quantum_index])
    {
        qset->quantum_array[quantum_index] = kmalloc(dev->quantum_size, GFP_KERNEL);
        if(!qset->quantum_array)
            goto out;
    }
    
    if(count > dev->quantum_size - quantum_offset)
        count = dev->quantum_size - quantum_offset;
    
    if(copy_from_user(buf, qset->quantum_array[quantum_index] + quantum_offset, count))
    {
        retval = -EFAULT;
        goto out;
    }
    
    *f_pos += count;
    retval = count;
    
    if(dev->size < *f_pos)
        dev->size = *f_pos;
    
out:
    up(&dev->sem);
    return retval;
}

























