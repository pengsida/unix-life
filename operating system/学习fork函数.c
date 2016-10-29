fork函数功能描述
{
    fork()系统调用用于创建子进程。
    fork函数是sys_fork()系统调用的辅助处理函数集。
    内存验证函数verify_area()中验证内存的起始位置和范围的调整示意图。因为内存写验证函数write_verify()需要以内存页面为单位进行操作，因此在调用write_verify()之前，需要把验证的起始位置调整为页面起始位置，同时对验证范围作相应调整。
}

fork函数源码
{
    #include<errno.h>
    #include<linux/sched.h>
    #include<linux/kernel.h>
    #include<asm/segment.h>
    #include<asm/system.h>

    extern void write_verify(unsigned long address); // 内存写验证函数write_verify()
    int find_empty_process(void);
    long last_pid = 0; // 最新进程号，其值由get_empty_process()生成

    // 用于验证内存的起始位置，并且对范围作相应调整。
    void verify_area(void *addr, int size)
    {
        unsigned long start;
        start = (unsigned long) addr; 
        size += start & 0xfff; // 首先获得起始位置addr在所在页面中的偏移值，原范围加上这一偏移值，扩展成从所在页面起始位置开始的范围值
        start &= 0xfffff000; // 让start变为当前页面起始地址
        start += get_base(current->ldt[2]); // 把start加上进程数据段在线性地址空间中的起始基址，变成整个线性空间中的地址位置
        while(size>0)
        {
            size -= 4096;
            write_verify(start); // 循环进行写页面验证
            start += 4096;
        }
    }

    // 内存分配函数
    int copy_mem(int nr, struct task_struct * p)
    {
        unsigned long old_data_base, new_data_base, data_limit;
        unsigned long old_code_base, new_code_base, code_limit;
        // 获取当前进程局部描述符表中代码段描述符合数据段描述符项中的段限长
        code_limit = get_limit(0x0f); // 0x0f是代码段选择符
        data_limit = get_limit(0x17); // 0x17是数据段选择符
        // 获取当前进程代码段和数据段在线性地址空间中的基地址
        old_code_base = get_base(current->ldt[2]);
        old_data_base = get_base(current->ldt[1]);
        if(old_data_base != old_code_base)
            panic("We don't support seperate I&D");
        if(data_limit < code_limit)
            panic("Bad data_limit");
        new_data_base = new_code_base = nr * 0x4000000; // 任务号 * 64MB
        p->start_code = new_code_base; 
        // 设置新进程局部描述符表中的基地址
        set_base(p->ldt[1], new_code_base);
        set_base(p->ldt[2], new_data_base);
        // 设置新进程的页目录表项和页表项
        // copy_page_tables在正常情况下返回0，否则表示出错，释放刚刚申请的页表项
        if(copy_page_tables(old_data_base, new_data_base, data_limit))
        {
            free_page_tables(new_data_base, data_limit);
            return -ENOMEM;
        }
        return 0;
    }

    // 下面是主要的fork子程序

    // 复制进程。该函数的参数是进入系统调用中断处理过程开始，直到调用本系统调用处理过程和调用本函数前时逐步压入栈的各寄存器值。
    // 参数分别是，CPU执行中断指令压入的用户栈地址ss和esp，标志寄存器eflags和返回地址cs和eip
    // 刚进入system_call时压入栈的段寄存器ds、es、fs和edx、ecx、ebx
    // 调用sys_call_table中sys_fork函数时压入栈的返回地址none
    // 调用copy_process之前压入栈的gs、esi、ebp和eax(存放nr值，nr是调用find_empty_process()分配的任务数组项号)
    int copy_process(int nr, long edp, long edi, long esi, long gs, long none, long ebx, long ecx, long edx, long fs, long es, long ds, long eip, long cs, long eflags, long esp, long ss)
    {
        struct task_struct *p;
        int i;
        struct file *f;
        // 为新任务数据结构分配内存
        p = (struct task_struct*)get_free_page();
        if(!p)
            return -EAGAIN;
        // 将新任务结构指针放入任务数组的nr项中。nr为任务号，由find_empty_process()返回
        task[nr] = p;
        *p = *current;
        p->state = TASK_UNINTERRUPTIBLE; // 将新进程的状态置为不可中断等待状态，以防止内核调度其执行
        p->pid = last_pid; // 新进程号
        p->father = current->pid; // 设置父进程号
        p->counter = p->priority; // 运行时间片值
        p->signal = 0; // 信号位置0
        p->alarm = 0; // 报警定时值置0
        p->leader = 0; // 进程的领导权
        p->utime = p->stime = 0; // 用于记录用户态和核心态运行时间
        p->cutime = p->cstime = 0; // 用于记录子进程用户态和核心态运行时间
        p->start_time = jiffies; // 进程开始运行时间，数值为当前时间嘀嗒数
        // 修改任务状态段TSS数据
        p->tss.back_link = 0;
        p->tss.esp0 = PAGE_SIZE + (long)p; // 任务内核态栈指针。由于系统给任务结构p分配了一页新内存，所以(PAGE_SIZE+long(p))让esp0指向该页顶端
        p->tss.ss0 = 0x10;  // ss0是内核态栈的段选择符，ss0:esp0代表了内核态的栈
        p->tss.eip = eip; // 指令代码指针
        p->tss.eflags = eflags; // 修改标志寄存器
        p->tss.eax = 0; // 新进程会返回0的原因
        p->tss.ecx = ecx;
        p->tss.edx = edx;
        p->tss.ebx = ebx;
        p->tss.esp = esp;
        p->tss.ebp = ebp;
        p->tss.esi = esi;
        p->tss.edi = edi;
        p->tss.es = es & 0xffff; // 段寄存器仅16位有效
        p->tss.cs = cs & 0xffff;
        p->tss.ss = ss & 0xffff;
        p->tss.ds = ds & 0xffff;
        p->tss.fs = fs & 0xffff;
        p->tss.gs = gs & 0xffff;
        p->tss.ldt = _LDT(nr); // 把GDT中本任务LDT段描述符的选择符保存在本任务的TSS段中。当CPU执行切换任务时，会自动从TSS中把LDT段描述符的选择符加载到ldtr寄存器中
        p->tss.trace_bitmap = 0x80000000;
    }
}