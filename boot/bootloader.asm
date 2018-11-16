
[org 0x7c00]
; 获取可用的内存数量
mov ax, 0
mov ds, ax
mov ah, 0x88
int 0x15 ; 返回AX, 1KB为单位, AX * 1K 代表在1MB之上可用的内存数量
mov [0x500], ax ; // 存入0x500这个地址, 给内核使用

; 将内核加载
; 从0x1000开始放置数据
readSectorNumbers equ 50 ; 要读取的扇区个数
mov ah, 0x02
mov dl, 0x80 ; 从硬盘用0x80, 软盘采用0
mov ch, 0
mov dh, 0
mov cl, 2 ; 从第2个扇区开始读, 扇区的编号是从1开始的, 不是从0开始
mov al, readSectorNumbers ; readSectorNumbers 个扇区
; 设置[es:bx]指向0x1000
mov bx, 0
mov es, bx
mov bx, 0x1000
int 0x13

; 开启保护模式
lgdt [gdt_descriptor] ; 载入gdt

; 让cpu能寻址4 GB
in al,0x92                         ;南桥芯片内的端口 
or al,0000_0010B
out 0x92,al                        ;打开A20

; 设置PE位, 开启保护模式
cli ; 关中断
mov eax, cr0
or eax, 1
mov cr0, eax

jmp 1_0_00b:protectModeCode ; 跳转到保护模式的代码, cs从GDT中选第1个段选择子
%include "GDT.asm" ; GDT here.

[bits 32]
protectModeCode:
    ; 数据段选择子
    mov ax, 10_0_00b
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 栈选择子
    mov ax, 11_0_00b
    mov ss, ax
    mov esp, 0xf000

    jmp 1_0_00b:0x1000 ; 跳至内核代码起始处
    
times 510 - ($ - $$) db 0
db 0x55, 0xAA