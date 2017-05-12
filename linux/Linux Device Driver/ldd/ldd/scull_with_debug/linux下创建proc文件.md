# linux下创建proc文件

## /proc文件的介绍

/proc 文件系统是一个虚拟文件系统，通过它可以使用一种新的方法在 Linux内核空间和用户空间之间进行通信。在 /proc 文件系统中，我们可以将对虚拟文件的读写作为与内核中实体进行通信的一种手段，但是与普通文件不同的是，这些虚拟文件的内容都是动态创建的。

大家对“Linux内核空间和用户空间之间进行通信”这个概念可能还没有全面的认识，接下来我举两个例子。

比如现在我想查看linux下有哪些设备，这肯定就是linux内核空间的信息，而我们现在处于用户空间，当然无法随意查看。
现在就可以通过/proc/devices查看：

```bash
root@scholes:/home/pengsida# cat /proc/devices 
Character devices:
  1 mem
...

Block devices:
259 blkext
...
```

或者我们想查看linux下有哪些模块，这也是linux内核空间的信息，不过我们也可以通过/proc文件系统轻松查看：
```bash
root@scholes:/home/pengsida# cat /proc/modules 
snd_hda_codec_hdmi 45056 1 - Live 0xffffffffc0b9a000
intel_rapl 20480 0 - Live 0xffffffffc0cdf000
x86_pkg_temp_thermal 16384 0 - Live 0xffffffffc0c12000
...
```

## 对/proc文件系统的需求

我们通过上面的操作可以感受到/proc文件系统很好用，可以轻松获得一些内核空间的信息。
其实这个和/var/log/syslog有点类似，只是/proc文件系统可以针对特定的设备，而/var/log/syslog是内核信息的总汇。

当我们开发linux驱动程序的时候，有时候我们可能会想获得一些信息。
如果只是借助printk，然后再去/var/log/syslog查看内核信息，难免有些麻烦，所以我们这里可以针对我们的驱动程序写一个/proc文件。

## 创建/proc文件的流程

因为代码有点长，所以我这里打算先讲思路，然后举出各部分的代码片段，完整代码请参考附件。

对于创建/proc文件，linux系统有给出相应的接口，我们只要按照如下步骤进行编写，就可以创建一个/proc文件：

1. 首先创建四个函数，用于迭代器操作，也就是用于初始化struct seq_operations这个结构体，这个结构体的定义如下：

    ```c
    struct seq_operations {
      void * (*start) (struct seq_file *m, loff_t *pos);
      void (*stop) (struct seq_file *m, void *v);
      void * (*next) (struct seq_file *m, void *v, loff_t *pos);
      int (*show) (struct seq_file *m, void *v);
    };
    ```

    所以我们在这里就应该相应的创建四个函数start、stop、next和show，然后初始化struct seq_operations结构体

2. 然后声明一个针对/proc文件的struct file_operations结构体。这个struct file_operations结构体有五个成员必须赋值，
分别是owner、open、release、read和llseak，其中release、read和llseak都有相应的seq_*函数，而open函数需要我们自己定义。

   之所以要自己定义open函数，是因为我们要在这个自定义函数中，把之前初始化的struct seq_operations和struct file关联起来，如下：

   ```c
   int self_open(struct inode* file_inode, struct file* file_ptr)
   {
     return seq_open(file_ptr, &self_seq_operations);
   }
   ```

   这里的seq_open()函数是linux内核函数，原型如下：
   ```c
   int seq_open(struct file *file, const struct seq_operations *op);
   ```

3. 最后通过proc_create()函数创建一个/proc文件，并将这个/proc文件与上面定义的struct file_operations关联起来，
proc_create()是linux内核函数，原型如下：

   ```c
   /*
   @name: /proc文件的文件名
   @mode: 文件操作模式
   @parent：文件父目录，通常填NULL
   @proc_fops：要关联的struct file_operations
   */
   static inline struct proc_dir_entry *proc_create(
    const char *name, umode_t mode, struct proc_dir_entry *parent,
    const struct file_operations *proc_fops);
   ```

4. 当然，如果我们卸载模块的时候，当然也要记得删除相应的/proc文件，内核函数如下：
   ```c
   /*
    * Remove a /proc entry and free it if it's not currently in use.
    */
   void remove_proc_entry(const char *name, struct proc_dir_entry *parent);
   ```

## 查看/proc文件的效果

这里我为上次编写的scull驱动程序创建了/proc/scull_seq文件，效果如下：

```bash
root@scholes:/home/pengsida/ldd/scull_psd# ll > /dev/scull_psd0
root@scholes:/home/pengsida/ldd/scull_psd# cat /proc/scull_seq 
The scull_device number is 0
The scull_device total size is 1282
The quantum number is 128, the quantum size is 64

We are in 0 qset, the qset address is ffff909da897f5a0
The 0 quantum address is ffff909da4ead280
The 1 quantum address is ffff909da4ead180
The 2 quantum address is ffff909da4ead1c0
...
```

## 关键代码

### 四个迭代器操作函数

这里四个函数都挺简单的，如果大家对迭代器有所了解，这四个函数肯定一读就懂。我这里给出自己对迭代器的理解：我们有时候需要
遍历自己定义的数据结构，这时我们可以给出一个接口函数，通常名字叫做next()，然后定义一个相应的数据结构，也就是迭代器。
当调用next()函数的时候，返回指向下一个数据结构的迭代器，比较简单的迭代器实现可以就是数据结构的指针。

还需要说明的有两点：

1. struct seq_file结构体代表着/proc/scull_seq文件，只有要对/proc/scull_seq进行读写操作的时候才用得到。
2. seq_printf()是专门向/proc/scull_seq写内容的函数。

```c
void* scull_seq_start(struct seq_file* sfile, loff_t* pos)
{
    if(*pos >= scull_nr_device)
    {
        SCULL_PRINT_DEBUG("SCULL in scull_seq_start: "
          "*pos is bigger than scull_nr_device\n");

        *pos = 0;
        return NULL;
    }
    
    return scull_devices + *pos;
}

void scull_seq_stop(struct seq_file* sfile, void* v)
{
    return;
}

void* scull_seq_next(struct seq_file* sfile, void* v, loff_t* pos)
{
    *pos = *pos + 1;
    
    if (*pos >= scull_nr_device)
    {
        SCULL_PRINT_DEBUG("SCULL in scull_seq_next: "
          "*pos is bigger than scull_nr_device\n");

        *pos = 0;
        return NULL;
    }
    
    return scull_devices + *pos;
}

int scull_seq_show(struct seq_file* sfile, void* v)
{
    struct scull_dev* scull_device = (struct scull_dev*)v;
    struct scull_qset* qset_list = NULL;
    int i = 0;
    int j = 0;
    
    if (down_interruptible(&scull_device->sem))
        return -ERESTARTSYS;
    
    seq_printf(sfile, "The scull_device number is %d\n",
     (int)(scull_device - scull_devices));

    seq_printf(sfile, "The scull_device total size is %d\n",
     scull_device->total_size);

    seq_printf(sfile, "The quantum number is %d, the quantum size is %d\n",
      scull_device->quantum_num, scull_device->quantum_size);

    seq_printf(sfile, "\n");
    
    for (qset_list = scull_device->qset_list; qset_list; qset_list = qset_list->next, i++)
    {
        seq_printf(sfile, "We are in %d qset, the qset address is %p\n", 
          i, qset_list);
        
        for (j = 0; j < scull_device->quantum_num; j++)
        {
            seq_printf(sfile, "The %d quantum address is %p\n",
             j, qset_list->quantnum_array[j]);
        }
        
        seq_printf(sfile, "\n");
    }
    
    seq_printf(sfile, "-------------------"
    "-----------------------------------\n");
    
    up(&scull_device->sem);
    
    return 0;
}
```

### 定义struct seq_operations
```c
struct seq_operations scull_seq_operation = {
    .start = scull_seq_start,
    .stop = scull_seq_stop,
    .next = scull_seq_next,
    .show = scull_seq_show,
};
```

### 定义struct file_operations
```c
int scull_seq_file_open(struct inode* file_inode, struct file* file_ptr)
{
    return seq_open(file_ptr, &scull_seq_operation);
}

struct file_operations scull_seq_file_operation = {
    .owner = THIS_MODULE,
    .open = scull_seq_file_open,
    .release = seq_release,
    .read = seq_read,
    .llseek = seq_lseek,
};
```

### 创建/proc/scull_seq文件
```c
void scull_create_proc_file(void)
{
    proc_create("scull_seq", 0, NULL, &scull_seq_file_operation);
}
```

### 删除/proc/scull_seq文件
```c
void scull_remove_proc_file(void)
{
    remove_proc_entry("scull_seq", NULL);
}
```