%macro  Descriptor  3
        dw  %2 & 0FFFFh ; dw = define word 段限长1
        dw  %1 & 0FFFFh ; 段基址1
        db  (%1 >> 16) & 0FFh ; db = define byte 段基址2
        dw  ((%2 >> 8) & 0F00h) | (%3 & 0F0FFh) ; 属性1+段界限2+属性2 
        db  (%1 >> 24) & 0FFh ; 段基址3
%endmacro