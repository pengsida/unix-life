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

## 完整代码

## Makefile文件