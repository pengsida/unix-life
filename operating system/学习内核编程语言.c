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

        entry start
        start:
                jmpi    go,BOOTSEG
        go:     mov ax,cs
                mov ds,ax
                mov es,ax
                mov [msg1+17],ah
                mov cx,#20
                mov dx,#0x1004
                mov bx,#0x000c
                mov bp,#msg1
                mov ax,#0x1301
                int 0x10
        loop1:  jmp loop1
        msg1:   .ascii  "Loading system..."
                .byte   13,10
        .org    510
                .word   0xAA55
        .text
        endtext:
        .data
        enddata:
        .bss
        endbss:
    }
}