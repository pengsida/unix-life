#include "apue.h"

// 需要确保malloc函数返回的存储空间满足将要保存的对象的对齐要求
// 假设该机器最受限的类型是long类型
// 下面实现按照long类型的边界对齐
// union的使用解决了地址的对齐问题
typedef long Align;
union header
{
    struct
    {
        union header *ptr;
        unsigned size; // 用于记录实际分配的块的大小。因为malloc函数控制的块不一定是连续的，这样就不可能通过指针算术运算计算其大小。
    } s;
    Align x; // 通过这个来强制块按照long类型的边界对齐
    // Align字段永远不会被使用，它仅仅用于强制每个头部在最坏的情况下满足对齐的要求。
};

typedef union header Header;
static Header base; // base是空闲块链表的头部
static Header *freep = NULL; // 第一次调用malloc函数的时候，freep为NULL，系统将创建一个退化的空闲块链表，它只包含一个大小为0的块，且该块指向它自己。

static Header *morecore(unsigned); // 函数morecore用于向操作系统请求存储空间，其实现细节因系统的不同而不同。

void *malloc(unsigned nbytes)
{
    Header *p, *prevp;
    unsigned nunits;
    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1; // 计算需要分配的块大小
    if((prevp = freep) == NULL) // 每次请求空闲空间时，都将搜索空闲块链表。搜索从上一次找到空闲块的地方freep开始。这种策略可以保证链表是均匀的。
    {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr)
    {
        if(p->s.size >= nunits)
        {
            if(p->s.size == nunits)
                prevp->s.ptr = p->s.ptr;
            else
            {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void*)(p+1); // 返回给用户的指针都指向块内的空闲存储空间，即比指向头部的指针大一个单元。
        }
        if(p == freep)
            if((p = morecore(nunits)) == NULL) 
                return NULL;
    }
}

// 将块ap放入空闲块链表中
void free(void* ap) 
{
    Header *bp, *p;
    bp = (Header*)ap - 1; // 指向块头
    for(p = freep; !(bp > p && bp < p->s.ptr)/*可以插入空闲块的地方可能在两个空闲块之间*/; p = p->s.ptr) // 从freep指向的地址开始，逐个扫描空闲块链表，寻找可以插入空闲块的地方。
        if(p >= p->s.ptr && (bp > p || bp < p->s.ptr)) // 处理被释放的块在链表的开头或末尾的情况
            break;
    
    // 如果被释放的块与另一空闲块相邻，则将这两个块合并起来
    if(bp + bp->s.size == p->s.ptr) // 与上一相邻块合并 
    {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr; // 将被释放的块的指针指向可插入位置的空闲块的下一个空闲块
    }
    else
        bp->s.ptr = p->s.ptr;
    if(p + p->s.size == bp) // 与下一相邻块合并
    {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else
        p->s.ptr = bp;
    freep = p;
}

int main()
{
    return 0;
}

#define NALLOC 1024

static Header *morecore(unsigned nu)
{
    char *cp, *sbrk(int);
    Header *up;
    if(nu < NALLOC) // 因为向系统请求存储空间是一个开销很大的操作，为了不让malloc函数每次都执行该操作，基于这个考虑，morecore函数请求至少NALLOC个单元。
        nu = NALLOC; 
    cp = sbrk(nu * sizeof(Header)); // unix系统调用sbrk(n)返回一个指针，该指针指向n个字节的存储空间。如果没有存储空间，sbrk调用返回-1.
    if(cp == (char*)-1) // 只有在一般指针间的比较操作有意义的机器上，这条比较语句才有意义
        return NULL;
    up = (Header*)cp;
    up->s.size = nu; 
    free((void*)(up+1)); // 在设置完size字段后，morecore函数调用free函数把多余的存储空间插入到空闲区域中。
    return freep;
}