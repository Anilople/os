# do not use 64 bit environment

# like ORG in nasm
kernelORG = 0x1000

os.bin: boot/bootloader.bin kernel/kernel.bin
	cat $^ > $@
#  bs=512 count=10
run: os.bin test.vhd
	dd if=os.bin of=test.vhd conv=notrunc
	qemu-system-i386 test.vhd

# boot loader
boot/bootloader.bin: boot/bootloader.asm boot/GDT.asm
	nasm $< -o $@ -f bin -i ./boot/

# kernel
kernel/kernel.bin: kernel/kernel_entry.o kernel/kernel.o library/library.o screen/screen.o drivers/drivers.o 
	ld $^ -o $@ -Ttext $(kernelORG) --oformat binary

# jmp to main function in c
kernel/kernel_entry.o: kernel/kernel_entry.asm
	nasm $^ -o $@ -f elf

kernel/kernel.o: kernel/kernel.c
	gcc $^ -o $@ -c -ffreestanding -m32

# library
library/library.o: library/library.c library/library.h
	gcc $< -o $@ -c -ffreestanding -m32

# screen
screen/screen.o: screen/screen.c screen/screen.h
	gcc $< -o $@ -c -ffreestanding -m32

# drivers
drivers/drivers.o: drivers/ports.o drivers/disk.o
	ld $^ -o $@ -r

drivers/disk.o: drivers/disk.c drivers/disk.h
	gcc $< -o $@ -c -ffreestanding -m32

drivers/ports.o: drivers/ports.c drivers/ports.h
	gcc $< -o $@ -c -ffreestanding -m32
	
clean:
	rm -fr *.o *.bin
	rm -fr ./boot/*.o ./boot/*.bin
	rm -fr ./library/*.o ./library/*.bin
	rm -fr ./screen/*.o ./screen/*.bin
	rm -fr ./drivers/*.o ./drivers/*.bin
	rm -fr ./kernel/*.o ./kernel/*.bin