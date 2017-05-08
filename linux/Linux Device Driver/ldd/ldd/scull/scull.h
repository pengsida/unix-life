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
#include <linux/fs.h>

int scull_open(struct inode *, struct file *);
int scull_release(struct inode *, struct file *);
ssize_t scull_read(struct file *, char __user *, size_t, loff_t *);
ssize_t scull_write(struct file *, const char __user *, size_t, loff_t *);

#endif /* scull_h */
