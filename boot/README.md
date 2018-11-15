## boot loader
after command
```shell
nasm -f bin bootloader.asm -o bootloader.bin
```

File `bootloader.bin` is a binary file.

The size of `bootloader.bin` is 512 Bytes.

`BIOS` will copy its content to memory address `0x7c00`, and jump to `0x7c00` to execute it.

