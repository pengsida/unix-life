%include "pm.inc"

org 0100h
    jmp LABEL_BEGIN

[SECTION .gdt]

LABEL_GDT: Descriptor 0,0,0
LABEL_DESC_NORMAL: Descriptor 0,0ffffh,DA_DRW
LABEL_DESC_CODE32: Descriptor 0,SegCode32Len-1,DA_C+DA_32
LABEL_DESC_CODE16: Descriptor 0,0ffffh,DA_C
LABEL_DESC_DATA: Descriptor 0,DataLen-1,DA_DRW
LABEL_DESC_STACK: Descriptor 0,TopOfStack,DA_DRWA+DA_32
LABEL_DESC_TEST: Descriptor 0500000h,0ffffh,DA_DRW
LABEL_DESC_VIDEO: Descriptor 0B8000h,0ffffh,DA_DRW

GdtLen equ $-LABEL_GDT
GdtPtr equ dw GdtLen-1
           dd 0

SelectorNormal equ LABEL_DESC_NORMAL-LABEL_GDT
SelectorCode32 equ LABEL_DESC_CODE32-LABEL_GDT
SelectorCode16 equ LABEL_DESC_CODE16-LABEL_GDT
SelectorData equ LABEL_DESC_DATA-LABEL_GDT
SelectorStack equ LABEL_DESC_STACK-LABEL_GDT
SelectorTest equ LABEL_DESC_TEST-LABEL_GDT
SelectorVideo equ LABEL_DESC_VIDEO-LABEL_GDT

[SECTION .data1]
ALIGN 32
[BITS 32]
LABEL_DATA:
    SPValueInRealMode dw 0
; 在NASM中，变量和标签是一样的
; 下面这句等价于 PMMessage db "In protect mode now",0
PMMessage: db "In protect mode now",0
    OffsetPMMessage equ PMMessage-$$
StrTest: db "ABCDEFGHJKLMNOPQRSTUVWXYZ",0
    OffsetStrTest equ StrTest-$$
DataLen equ $-LABEL_DATA

[SECTION .gs]
ALIGN 32
[BITS 32]
LABEL_STACK:
    times 512 db 0
TopOfStack equ $-LABEL_STACK-1

[SECTION .s16]
[BITS 16]
LABEL_BEGIN:
    ; cs存放着代码段的段基址
    mov ax,cs
    mov es,ax
    mov ds,ax
    mov ss,ax
    mov sp,0100h

    mov [LABEL_GO_BACK_TO_REAL+3],ax
    mov [SPValueInRealMode],sp

    mov ax,cs
    ; 将我们的源操作数取出来,然后置于目的操作数,目的操作数其余位用0填充
    movzx eax,ax
    ; 另一种实现方法
    ; xor eax,eax
    ; mov ax,cs 
    shl eax,4
    add eax,LABEL_SEG_CODE16
    mov word [LABEL_DESC_CODE16+2],ax
    shr eax,16
    mov byte [LABEL_DESC_CODE16+4],al
    mov byte [LABEL_DESC_CODE16+7],ah

    xor eax,eax
    mov ax,cs
    shl eax,4
    add eax,LABEL_SEG_CODE32
    mov word [LABEL_DESC_CODE32+2],ax
    shr eax,16
    mov byte [LABEL_DESC_CODE32+4],al
    mov byte [LABEL_DESC_CODE32+7],ah

    xor eax,eax
    ; ds存放着数据段的段基址
    mov ax,ds
    shl eax,4
    add eax,LABEL_DATA
    mov word [LABEL_DESC_DATA+2],ax
    shr eax,16
    mov byte [LABEL_DESC_DATA+4],al
    mov byte [LABEL_DESC_DATA+7],ah

    xor eax,eax
    mov ax,ds
    shl eax,4
    add eax,LABEL_STACK
    mov word [LABEL_DESC_DATA+2],ax
    shr eax,16
    mov byte [LABEL_DESC_DATA+4],al
    mov byte [LABEL_DESC_DATA+7],ah

    ; 初始化指向GDT表的指针
    xor eax,eax
    mov ax,ds
    shl eax,4
    add eax,LABEL_GDT
    mov dword [GdtPtr+2],eax

    lgdt [GdtPtr]

    cli

    ; 打开A20地址线
    in al,92h
    or al,00000010b
    out 92h,al

    ; 修改CR0的PE位
    mov eax,cr0
    or eax,1
    mov cr0,eax
    sti

    jmp dword SelectorCode32:0

LABEL_REAL_ENTRY:
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov sp,[SPValueInRealMode]

    in al,92h
    and al,11111101b
    out 92h,al
    sti

    mov ax,4c00h
    int 21h ; 返回DOS

[SECTION .s32]
[BITS 32]
LABEL_SEG_CODE32:
    ; 加载数据段描述符的选择符
    mov ax,SelectorData
    mov ds,ax
    ; 加载测试段描述符的选择符
    mov ax,SelectorTest
    mov es,ax
    ; 加载视频段描述符的选择符
    mov ax,SelectorVideo
    mov gs,ax
    ; 加载堆栈段描述符的选择符
    mov ax,SelectorStack
    mov ss,ax

    ; 加载堆栈段的段指针，它是向下扩展的
    mov esp,TopOfStack

    mov ah,0Ch
    ; esi和edi是x86中的变址寄存器
    xor esi,esi
    xor edi,edi
    mov esi,OffsetPMMessage
    mov edi,(80 + 10 + 0) * 2
    ; 设置从低地址向高地址传送
    cld
.1:
    ; 相当于 load ES:ESI to al, ESI将自动增加
    lodsb
    ; Performs a bit-wise logical AND of the two operands
    ; The result of a bit-wise logical AND is 1 if the value of that bit in both operands is 1; otherwise, the result is 0.
    ; The OF and CF flags are cleared; SF, ZF and PF flags are set according to the result.
    test al,al
    ; JZ，当ZF=1时跳转
    ; 当ZF=1时，说明指向测试数据段已经到了尽头
    ; 用来测试一方寄存器是否为空
    jz .2
    mov [gs:edi],ax
    add edi,2
    jmp .1
.2:
    call DispReturn
    call TestRead
    call TestWrite
    call TestRead

    jmp SelectorCode16:0

TestRead:
    xor esi,esi
    mov ecx,8
.loop:
    mov al,[es:esi]
    call DipsAL
    inc esi
    ; loop decrements the count register
    loop .loop
    call DispReturn
    ret

TestWrite:
    push esi
    push edi
    xor esi,esi
    xor edi,edi
    mov esi,OffsetStrTest
    cld
.1:
    lodsb
    test al,al
    jz .2
    mov [es:edi],al
    inc edi
    jmp .1
.2:
    pop edi
    pop esi
    ret

; 用于显示al中的数字
DipsAL:
    push ecx
    push edx

    mov ah,0Ch
    mov dl,al
    shr al,4
    mov ecx,2
.begin:
    and al,01111b
    cmp al,9
    ja .1
    add al,'0'
    jmp .2
.1:
    sub al,0Ah
    add al,'A'
.2:
    mov [gs:edi],ax
    add edi,2

    mov al,dl
    loop .begin
    add edi,2

    pop edx
    pop ecx

    ret

; 模拟一个回车符的显示
DispReturn:
    push eax
    push ebx
    mov eax,edi
    mov bl,160
    div bl
    and eax,0FFh
    inc eax
    mov bl,160
    mul bl
    mov edi,eax
    pop ebx
    pop eax
    ret

SegCode32Len equ $-LABEL_SEG_CODE32

[SECTION .s16code]
ALIGN 32
[BITS 16]
LABEL_SEG_CODE16:
    cli

    mov ax,SelectorNormal
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    mov eax,cr0
    and al,11111110b
    mov cr0,eax

    ; 在16位代码段初始化的过程中有 mov [LABEL_GO_BACK_TO_REAL+3],ax
    ; LABEL_GO_BACK_TO_REAL是"jmp 0:LABEL_REAL_ENTRY"指令语句前的标号
    ; LABEL_GO_BACK_TO_REAL后第四个和第五个字节就是段地址所在
    ; 所以jmp指令中的段地址0就被改成了ax中的值
LABEL_GO_BACK_TO_REAL:
    jmp 0:LABEL_REAL_ENTRY
    