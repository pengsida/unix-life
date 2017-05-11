//
//  read_file.c
//  ldd
//
//  Created by pengsida on 2017/5/9.
//  Copyright © 2017年 pengsida. All rights reserved.
//

#include <linux/module.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/limits.h>
#include <linux/slab.h>
#include <asm/current.h>

static int get_specific_line(char* result, struct file* file_ptr, loff_t* pos)
{
    ssize_t count = 1;
    char buf[2] = {0};
    const char OBJ_M[] = "bj-m";
    int index = 0;
    memset(result, 0, 100);
    
    while (1)
    {
        if (vfs_read(file_ptr, buf, count, pos) <= 0)
        {
            printk(KERN_ALERT "READ_FILE: reach the end of file\n");
            break;
        }
        
        if (index < 4 && buf[0] != OBJ_M[index])
            break;
        
        if (buf[0] == '\n')
            return 0;
        
        result[index] = buf[0];
        
        index++;
        
        if (index >= 100)
        {
            printk(KERN_ALERT "READ_FILE: something wrong\n");
            break;
        }
    }
    
    return 1;
}

static void copy(char* from, char* to)
{
    while (*from && *to)
    {
        *to = *from;
        from++;
        to++;
    }
    *from='\0';
    *to='\0';
}

static void squeeze(char* result)
{
    while (*result)
    {
        if (*result == ' ')
            copy(result+1, result);
        result++;
    }
}

static void get_name(char* result)
{
    char* start = result;
    
    while (*result)
    {
        if (*result == '=')
        {
            copy(result+1, start);
            result = start;
            continue;
        }
        
        if (*result == '.')
        {
            *result = '\0';
            break;
        }
        
        result++;
    }
}

int read_file_init(void)
{
    struct file* file_ptr;
    mm_segment_t fs = get_fs();
    loff_t pos = 0;
    char buf[2] = {0, 0};
    char result[100] = {0};
    ssize_t count = 1;
    int times = 0;
    
    printk(KERN_ALERT "READ_FILE: read_file_init\n");
    
    file_ptr = filp_open("/home/pengsida/Makefile", O_RDONLY, 0644);
    if (IS_ERR(file_ptr))
    {
        printk(KERN_ALERT "READ_FILE: open file fail\n");
        return -EFAULT;
    }
    set_fs(KERNEL_DS);
    
    while (1)
    {
        
        if(vfs_read(file_ptr, buf, count, &pos) <= 0)
        {
            printk(KERN_ALERT "READ_FILE: reach the end of file\n");
            break;
        }
        
        if (buf[0] == 'o')
        {
            if(get_specific_line(result, file_ptr, &pos) == 0)
            {
                squeeze(result);
                get_name(result);
                printk(KERN_ALERT "READ_FILE right: %s\n", result);
            }
        }
        
        if (times >= 10000)
        {
            printk(KERN_ALERT "READ_FILE: something wrong\n");
            break;
        }
        
        times++;
    }
    
    filp_close(file_ptr, NULL);
    set_fs(fs);
    return 0;
}

void read_file_exit(void)
{
    printk(KERN_ALERT "READ_FILE: read_file_exit\n");
}

module_init(read_file_init);
module_exit(read_file_exit);
