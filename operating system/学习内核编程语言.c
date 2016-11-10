linux内核完全剖析
第三章 内核编程语言和环境

3.0 本章学习方法
{
    先大致浏览一下本章内容，然后就阅读随后章节，在遇到问题时在回头来参考本章内容。
}

3.1 as86汇编语言语法
{
    语句类型
    {
        1.空行
        2.赋值语句
        3.伪操作符语句
        4.机器指令语句
    }

    示例代码
    {
        .global begtext, begdata, begbss, endtext, enddata, endbss
        .text
        begtext:
        .data
        begdata:
        .bss
        begbss:
        .text
        BOOTSEG = 0x07c0

        entry start !告知链接程序，程序从start标号开始执行
        start:
                jmpi    go,BOOTSEG ;使用段间跳转语句，给CS赋值为0x07c0
        go:     mov ax,cs   !段寄存器cs值存入cs，用于初始化ds和es。
                mov ds,ax
                mov es,ax
                mov [msg1+17],ah ;使用了间接操作数
                mov cx,#20
                mov dx,#0x1004 !字符串将显示在第17行、第5列处
                mov bx,#0x000c !字符显示属性
                mov bp,#msg1 ;将msg1的地址值放入寄存器中
                mov ax,#0x1301
                int 0x10
        loop1:  jmp loop1
        msg1:   .ascii  "Loading system..."
                .byte   13,10 ;定义了回车和换行两个字符
        .org    510 !表示从这以后的语句从地址510开始存放
                .word   0xAA55
        .text
        endtext:
        .data
        enddata:
        .bss
        endbss:
    }
}

3.2 as汇编语言语法
{
    指令语句包含4个组成部分：标号，操作码，操作数和注释

    操作数
    {
        操作数可以是立即数、寄存器或内存

        立即操作数需要加一个$前缀
        寄存器名需要加一个%前缀
        内存操作数由变量名或者含有变量地址的一个寄存器指定
        间接操作数需要加一个*前缀
    }

    操作码
    {
        操作码可以添加一个字符后缀来指明操作数的宽度。字符'b','w'和'l'分别指定byte、word和long类型的操作数。
        操作吗前缀用于修饰随后的操作码
    }

    内存引用：section:disp(base, index, scale)

    区用于表示一个地址范围，操作系统将会以相同的方式对待和处理在该地址范围中的数据信息。
    链接器ld生成可执行程序时，ld会把程序中的字节块移动到程序运行时的地址处。这些块是作为固定单元进行移动的，它们的长度以及字节次序都不会被改变，这样的固定单元就被称作是区。
    
    重定位操作：为区分配运行时刻的地址的操作
    执行重定位操作，在每次涉及目标文件中的一个地址时，ld必须知道
    {
        1.目标文件中对一个地址的引用是从什么地方算起的
        2.该引用的字节长度是多少
        3.该地址引用的是哪个区？(地址)-(区的开始地址)的值等于多少？
        4.对地址的引用与程序计数器PC相关吗？
    }
    as中的所有地址都可表示为：(区)+(区中偏移)。
    使用记号{secname N}来表示区secname中偏移N
    
    绝对地址区：ld将各个目标文件组合在一起时，absolute区中的地址将始终不变。
    未定义区：任何地址，如果汇编时无法确定其所属的区，那么它就会被归入未定义区。

    链接器ld涉及的4类区
    {
        text区和data区：用于保存程序。text区保存指令代码和常数，数据不会改变，被进程所共享。data区的变量通常会改变。
        bss区：用于存放未初始化的变量。还可以作为公共变量的存储空间。
        absolute区：该区的地址0总是重定位到运行时刻的地址0处。
        undefined区：不属于前三者的区都属于这个区。
    }

    在.text或.data后面加上数值参数，如.data 0，则指定随后的语句被汇编到.data的0子区中。
    每个区都有一个位置计数器，它会对每个汇编进该区的字节进行计数。
    特殊符号'.'表示as汇编时的当前地址。
    标号是后面紧随一个冒号的符号，代表了活动位置计数器当前的值。
    bss区中所有字节内容在程序运行之前都将被清零。
    .lcomm命令用于在bss区中定义一个符号
    .comm用于在bss区中声明一个公共符号
    符号的值是从区开始到标号处的地址值
}

3.3 嵌入汇编
{
    嵌入汇编语句的基本格式
    {
        asm("汇编语句"
            : 输出寄存器
            : 输入寄存器
            : 会被修改的寄存器)
    }

    例子
    {
        #define get_seg_byte(seg, addr) \
        ({ \
            register char __res; \
            __asm__("push %%fs; \
                     mov %%ax, %%fs; \
                     movb %%fs : %2, %%al; \
                     pop %%fs" \
                     : "=a" (__res) \ //“=”表示这是输出寄存器
                     : "0" (seg), "m"(*(addr))); \
        __res;})
        // 功能：将fs:(*(addr))所指定的字节放入__res，作为该宏函数的返回值
    }

    如果希望汇编语句不被GCC优化所修改，需要在asm符号后面添加关键词volatile
    例子
    {
        asm volatile();
        __asm __volatile();
    }

    关键词volatile也可以放到函数名前修饰函数，用来通知gcc编译器该函数不会返回，也就是不会再返回到调用者代码中。

    例子
    {
        extern inline int strncmp(const char *cs, const char* ct, int count)
        {
            register int __res;
            __asm__("cld\n"
                    "1:\tdecl %3\n\t"
                    "js 2f\n\t"
                    "scasb\n\t"
                    "jne 3f\n\t"
                    "testb %%al, %%al\n\t"
                    "jne 1b\n"
                    "2:\txorl %%eax,%%eax\n\t"
                    "jmp 4f\n"
                    "j1 4f\n\t"
                    "negl %%eax\n"
                    "4:"
                    :"=a"(__res)
                    :"D"(cs),"S"(ct),"c"(count)
                    :"si","di","cx"
            );
            return __res;
        }
    }

    寄存器变量，可以把一些变量值放到CPU寄存器中，
    局部寄存器变量定义方式：register int res __asm__("ax"); 最好不要明确地引用该寄存器并且假设该寄存器肯定引用的是该变量值。
}

3.4 C与汇编程序的相互调用
{
    3.4.1 C函数调用机制
    {
        函数调用操作包括从一块代码到另一块代码之间的双向数据传递和执行控制转移。
        数据传递通过函数参数和返回值来进行。
        执行控制转移使用栈来支持。

        3.4.1.1 栈帧结构和控制转移权方式
        {
            栈被用来传递函数参数、存储返回信息、临时保存寄存器原有值。
            栈帧结构有两个指针，一个栈指针，还有一个帧指针。

            例子
            {
                c语言代码
                {
                    void swap(int *a, int *b)
                    {
                        int c;
                        c = *a;
                        *a = *b;
                        *b = c;
                    }

                    int main()
                    {
                        int a, b;
                        a = 16; b = 32;
                        swap(&a, &b);
                        return (a-b);
                    }
                }

                汇编代码
                {
                    .text

                    _swap:
                            pushl   %ebp
                            movl    %esp,%ebp
                            subl    $4,%esp
                            movl    8(%ebp),%eax
                            movl    (%eax),%ecx
                            movl    %ecx,-4(%ebp)
                            movl    8(%ebp),%eax
                            movl    12(%ebp),%edx
                            movl    (%edx),%ecx
                            movl    %ecx,(%eax)
                            movl    12(%ebp),%eax
                            movl    -4(%ebp),%ecx
                            movl    %ecx,(%eax)
                            leave ;恢复原ebp、esp值，即movl %ebp,%esp; popl %ebp;
                            ret

                    _main:
                            pushl   %ebp
                            movl    %esp,%ebp
                            subl    $8,%esp
                            movl    $16,-4(%ebp)
                            movl    $32,-8(%ebp)
                            leal    -8(%ebp),%eax
                            pushl   %eax
                            leal    -4(%ebp),%eax
                            pushl   %eax
                            call    _swap
                            movl    -4(%ebp),%eax
                            subl    -8(%ebp),%eax
                            leave
                            ret
                }
            }
        }
    }

    3.4.2 在汇编程序中调用c函数
    {
        使用CALL指令。
        如果我们没有专门为调用函数压入参数舅直接调用它的话，那个这个函数就会把EIP位置以上的栈中其他内容作为自己的参数使用。

        system_call.s调用copy_process()函数的例子
        {
            push    %gs
            pushl   %esi
            pushl   %edi
            pushl   %ebp
            pushl   %eax
            call    _copy_process
            addl    $20,%esp
        1:  ret
        }
    }

    3.4.3 在c程序中调用汇编函数
    {
        如果调用的汇编语言程序比较短，那么可以直接在C程序中使用上面介绍的内联汇编语句来实现。
        例子，调用汇编函数mywrite(fd, buf, count)显示信息，调用myadd(a, b, result)执行加运算
        {
            int main()
            {
                char buf[2048];
                int a,b,res;
                char * mystr = "Calculating...\n";
                char * emsg = "Error in adding\n";
                a = 5; b = 10;
                mywrite(1, mystr, strlen(mystr));
                if(myadd(a, b, &res))
                {
                    sprintf(buf, "The result is %d\n", res);
                }
                else
                    mywrite(1, emsg, strlen(emsg));
                return 0;
            }
        }
    }
}