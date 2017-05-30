# 目录

1. 关于rc.d
2. 关于rc.local

## 关于rc.d

所有启动脚本放置在 /etc/init.d下rc?.d中放置的是init.d中脚本的链接，命名格式是:
```
S{number}{name}
K{number}{name}
S开始的文件向脚本传递start参数
K开始的文件向脚本传递stop参数
number决定执行的顺序
```

当我们需要开机启动自己的脚本时，只需要将可执行脚本丢在/etc/init.d目录下，然后在/etc/rc.d/rc*.d中建立软链接即可

## 关于rc.local

使用者把需要开机自动运行的程序写在这个脚本里，就可以开机自启。

>在完成 run level 3 的服务启动后，如果我还有其他的动作想要完成时，举例来说， 我还想要寄一封 mail 给某个系统管理帐号，通知他，系统刚刚重新开机完毕，那么， 是否应该要制作一个 shell script 放置在 /etc/rc.d/init.d/ 里面，然后再以连结方式连结到 /etc/rc.d/rc3.d/ 里面呢？呵呵！当然不需要！还记得上一小节提到的 /etc/rc.d/rc.local 吧？ 这个档案就可以执行您自己想要执行的系统指令了。像不像早期 DOS 年代的 autoexec.bat 与 config.sys 呢？ ^_^
>
> 也就是说，我有任何想要在开机时就进行的工作时，直接将他写入 /etc/rc.local ， 那么该工作就会在开机的时候自动被载入喔！而不必等我们登入系统去启动呢！ 是否很方便啊！一般来说，鸟哥就很喜欢把自己制作的 shell script 完整档名写入 /etc/rc.d/rc.local ，如此一来，开机就会将我的 shell script 执行过，真是好棒那！

也就是说，在linux启动的最后阶段，系统会执行存于rc.local中的命令。

