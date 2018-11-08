; use this jmp to kernel entry
[bits 32]
[extern main]
call main

; when main finished
cli
hlt