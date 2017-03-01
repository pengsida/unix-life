org 07c00h

BaseOfStack equ 07c00h
BaseOfLoader equ 09000h
OffsetOfLoader equ 0100h

RootDirSectors equ 14 ; 根目录区占用了14扇区
SectorNoOfRootDirectory equ 19
SectorNoOfFAT1 equ 1
DeltaSectorNo equ 17

; FAT12引导扇区固有的头信息
; FAT12引导扇区格式已经在本节开头说明

; BS_jmpBoot，长度要求为3字节
; 因为jmp short LABEL_START指令是2字节，所以需要加个nop指令，使得长度为3字节
jmp short LABEL_START
nop

BS_OEM DB 'ForrestY' 
BPB_BytsPerSec DW 512
BPB_SecPerClus DB 1
BPB_RsvdSecCnt DW 1
BPB_NumFATs DB 2
BPB_RootEntCnt DW 224
BPB_TotSec16 DW 2880
BPB_Media DB 0xF0
BPB_FATSz16 DW 9
BPB_SecPerTrk DW 18
BPB_NumHeads DW 2
BPB_HiddSec DD 0
BPB_TotSec32 DD 0
BS_DrvNum DB 0
BS_Reservd1 DB 0
BS_BootSig DB 29h
BS_VolID DD 0
BS_VolLab DB 'pengsida001'
BS_FileSysType DB 'FAT12   '

LABEL_START:
    ; 给每个段寄存器赋初值为代码段基址
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; 给堆栈指针赋初值，指向栈顶
    mov sp, BaseOfStack

    ;mov ax, 0600h
    ;mov bx, 0700h
    ;mov cx, 0
    ;mov dx, 0184fh
    ;int 10h

    ; mov dh, 0
    ; call DispStr

    ; ah=00h时，int 13h的功能是复位软驱。使用dl来指定驱动器号
    xor ah, ah
    xor dl, dl
    int 13h

    ; 根目录区起始扇区号SectorNoOfRootDirector为19
    ; 因为是要搜索根目录区中的根目录条目，所以要从头遍历
    ; wSectorNo地址单元中存放着要读取的扇区号
    mov word [wSectorNo], SectorNoOfRootDirectory
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
    ; 检查是否已经将根目录区中的所有扇区都遍历完
    cmp word [wRootDirSizeForLoop], 0
    jz LABEL_NO_LOADERBIN
    ; 将循环数减一
    dec word [wRootDirSizeForLoop]
    ; int 13h将扇区内容读入es:bx指定的数据缓冲区
    mov ax, BaseOfLoader
    mov es, ax
    mov bx, OffsetOfLoader
    ; ReadSector将ax和cl中的值作为参数
    ; ReadSector将从第ax个扇区开始的cl个扇区读入ex:bx指定的数据缓冲区中
    mov ax, [wSectorNo]
    mov cl, 1
    call ReadSector

    ; ds存放着代码段段基址，si为LoaderFileName的偏移地址，ds:si就指向了代码段中定义的"LOADER  BIN"字符串
    mov si, LoaderFileName 
    ; es存放着数据缓冲区的基地址，di为存放扇区数据处的偏移地址，es:di就指向了扇区数据
    mov di, OffsetOfLoader
    cld ; 从低位到高位
    mov dx, 10h ; 一个扇区共有512字节，一个根目录条目为32字节，所以遍历一个扇区需要16次
LABEL_SEARCH_FOR_LOADERBIN:
    cmp dx, 0 ; 检查循环是否结束
    jz LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR
    dec dx
    ; 文件名为11个字节。只要检查根目录条目前11个字节与文件名字符串是否相等就行了
    mov cx, 11 
LABEL_CMP_FILENAME:
    cmp cx, 0 ; 检查循环是否结束
    jz LABEL_FILENAME_FOUND
    dec cx
    lodsb ; 将ds:si指向的字节读入al，且将si的值自动加一
    cmp al, byte [es:di] ; 比较两个地址处的字节是否相同
    jz LABEL_GO_ON
    jmp LABEL_DIFFERENT
LABEL_GO_ON:
    inc di ; 将di的值加一，指向下一个字节
    jmp LABEL_CMP_FILENAME
LABEL_DIFFERENT:
    ; 如果文件名中11个字节有一个不相等，就跳过这个根目录条目
    ; 这时候需要处理di和si
    ; di最多加11，这时候di也只是改变低4位。所以让di的低4位与0000b相与
    ; 因为根目录条目是32字节，所以di的值肯定是32的倍数，di的第4位肯定是0
    ; 让di的第4位到第7位与1110b相与，让di的第4位变为0，让di指向本条目开头
    ; 其实，因为di最多改变低4位，所以and di, 0FFF0h也能达到相同的效果 
    and di, 0FFE0h
    ; di加32，指向下一个根目录条目
    add di, 20h
    ; 让si重新指向文件名字符串的开头
    mov si, LoaderFileName
    jmp LABEL_SEARCH_FOR_LOADERBIN
LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    ; 当前扇区没有Loader模块，需要搜索下一个扇区的内容
    ; 将要读取的扇区号加一
    add word [wSectorNo], 1
    jmp LABEL_SEARCH_IN_ROOT_DIR_BEGIN
LABEL_NO_LOADERBIN:
    jmp $ ; 找不到Loader模块，程序陷入死循环
LABEL_FILENAME_FOUND:
    mov ax, RootDirSectors
    ; 让di指向根目录条目开头
    and di, 0FFE0h
    ; 让di指向此条目对应的开始簇号
    add di, 01Ah
    ; 将开始簇号存储在cx中
    mov cx, word [es:di]
    push cx ; 存储扇区在FAT表中的序号
    ; 一个簇有一个扇区，所以簇号就是扇区号。cx目前存放着扇区号
    ; 让cx与根目录区扇区数相加，并将结果存储在cx中
    ; 根目录区起始扇区号为19，数据区的第一个簇的簇号是2
    ; 所以cx加上19再减去2，就得到了相对扇区号
    add cx, ax
    add cx, DeltaSectorNo ; DeltaSectorNo equ 19
    mov ax, BaseOfLoader
    mov es, ax
    mov bx, OffsetOfLoader
    ; ax作为ReadSector函数的参数，存放着相对扇区号
    mov ax, cx
LABEL_GOON_LOADING_FILE:
    ; cl作为ReadSector函数的参数，存放着要读取的扇区数
    mov cl, 1
    call ReadSector
    ; 将扇区在FAT表中的项号存储在ax中
    pop ax 
    ; GetFATEntry返回后，ax存放着Loader模块下一个簇号
    call GetFATEntry
    ; 判断ax是否为0FFFh，如果是，说明当前扇区是Loader模块最后一个扇区
    cmp ax, 0FFFh
    jz LABEL_FILE_LOADED
    push ax
    mov dx, RootDirSectors
    add ax, dx
    add ax, DeltaSectorNo
    ; 让bx指向数据缓冲区的下一个512个字节的开头，用于存放Loader模块的下一个扇区
    add bx, [BPB_BytsPerSec]
    jmp LABEL_GOON_LOADING_FILE
LABEL_FILE_LOADED:
    ; 开始执行Loader模块的代码
    jmp BaseOfLoader:OffsetOfLoader

wRootDirSizeForLoop dw RootDirSectors ; 根目录区中的扇区数
wSectorNo dw 0 ; 用于存放要读取的扇区号
bOdd db 0 ; 判断FAT项是从字节中的第0位开始还是从第4位开始的

LoaderFileName db "LOADER  BIN", 0 ; 文件名一定是11字节

ReadSector:
    push bp
    mov bp, sp
    sub esp, 2
    ; bp-2地址单元中存放着要读取的扇区数
    mov byte [bp-2], cl
    push bx
    ; BPB_SecPerTrk是每磁道的扇区数
    mov bl, [BPB_SecPerTrk]
    ; ax中存放着要读取的扇区号
    ; ax/bl的商扇区所在的磁道号，存放在al中
    ; ax/bl是要读取的扇区相对于当前磁道的起始扇区号，存放在ah中
    div bl
    ; 磁道的扇区号从1开始，所以要将ah的值加一
    int ah
    ; 根据int 13h的要求，cl要存放相对于磁道的起始扇区号
    ; dh存放磁头号。软盘中的磁头号不是0就是1。
    ; 软盘的排列并不是按照我们所想象的"把0面先排完了再开始排1面“，而是交替排列的
    ; 偶数的磁道号的磁头号为0，奇数的磁道号的磁头号为1
    ; 所以将al中存放着总磁道号与1相与就可以得到磁头号
    ; ch存放磁道号。原先al存放着软盘总的磁道号，而软盘有两面，所以需要除以2，得到相对于当前磁头的磁道号
    mov cl, ah
    mov dh, al
    and dh, 1
    shr al, 1
    mov ch, al
    pop bx
    mov dl, [BS_DrvNum]
.GoOnReading:
    ; ah指定int 13h的工作模式
    mov ah, 2
    ; al存放着要读取的扇区数
    mov al, byte [bp-2]
    int 13h
    jc .GoOnReading ; 如果读取错误CF会被置为1，这里程序会重读，直到正确为止

    add esp, 2
    pop bp
    ret

; 将ax作为输入参数，指定FAT表中FAT项的序号
; 将ax作为返回参数，将文件下一个簇号放在ax中
GetFATEntry:
    push es
    push bx
    push ax
    mov BaseOfLoader
    sub ax, 0100h ; 在BaseOfLoader之前空出4k的空间用于存放FAT项所在的扇区
    mov es, ax
    pop ax
    mov byte [bOdd], 0
    mov bx, 3
    mul bx
    mov bx, 2
    div bx
    cmp dx, 0
    jz LABEL_EVEN
    mov byte [bOdd], 1
LABEL_EVEN:
    xor dx, dx
    mov bx, [BPB_BytsPerSec]
    div bx
    push dx
    mov bx, 0
    add ax, SectorNoOfFAT1
    mov cl, 2
    call ReadSector
    
    pop dx
    add dx, dx
    mov ax, [es:bx]
    cmp byte [bOdd], 1
    jnz LABEL_EVEN_2
    shr ax, 4
LABEL_EVEN_2:
    and ax, 0FFFh
LABEL_GET_FAT_ENTRY_OK:
    pop bx
    pop es
    ret