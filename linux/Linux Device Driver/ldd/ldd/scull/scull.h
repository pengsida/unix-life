//
//  scull.h
//  ldd
//
//  Created by pengsida on 2017/4/29.
//  Copyright © 2017年 pengsida. All rights reserved.
//

#ifndef scull_h
#define scull_h

#include <linux/types.h>


int scull_init(int scull_minor);
static void scull_setup_cdev(struct scull_dev *dev, int index);

int scull_open(struct inode* node, struct file* filp);
int scull_release(struct inode* inode, struct file* filp);
ssize_t scull_read(struct file* filp, char __user* buf, size_t count, loff_t* f_pos);
ssize_t scull_write(struct file* filp, const char __user* buf, size_t count, loff_t* f_pos);

#endif /* scull_h */
