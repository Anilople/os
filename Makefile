# do not use 64 bit environment

CFLAGS := -c -ffreestanding -m32 -Wall

os.bin: boot/bootloader.bin kernel/kernel.bin
	cat $^ > $@
#  bs=512 count=10
run: os.bin test.vhd
	dd if=os.bin of=test.vhd conv=notrunc
	qemu-system-i386 test.vhd


###########################################################
# boot loader
boot/bootloader.bin: boot/bootloader.asm boot/GDT.asm
	nasm $< -o $@ -f bin -i ./boot/
###########################################################
# input and output
#############################
# port
portDirectory := io/port
$(portDirectory)/port.o : $(portDirectory)/port.c $(portDirectory)/port.h
	gcc $< -o $@ $(CFLAGS)
#############################
# screen
screenDirectory := io/screen
$(screenDirectory)/screen.o : $(screenDirectory)/screen.c $(screenDirectory)/screen.h $(portDirectory)/port.o
	gcc $< -o $@ $(CFLAGS)
#############################
# disk
diskDirectory := io/disk
$(diskDirectory)/disk.o : $(diskDirectory)/disk.c $(diskDirectory)/disk.h $(portDirectory)/port.o
	gcc $< -o $@ $(CFLAGS)
#############################
# cpu
cpuDirectory := io/cpu
# low level assembly for c programming language
$(cpuDirectory)/lowLevel.o : $(cpuDirectory)/lowLevel.c $(cpuDirectory)/lowLevel.h
	gcc $< -o $@ $(CFLAGS)
# descriptors, include GDT LDT IDT etc.
# IDT
idtDirectory := $(cpuDirectory)/descriptors
$(idtDirectory)/idt.o : $(idtDirectory)/idt.c $(idtDirectory)/idt.h
	gcc $< -o $@ $(CFLAGS)
# paging
$(cpuDirectory)/paging.o : $(cpuDirectory)/paging.c $(cpuDirectory)/paging.h
	gcc $< -o $@ $(CFLAGS)
# all object file to cpu.o, "-r" mean keep the symbols
$(cpuDirectory)/cpu.o : $(cpuDirectory)/lowLevel.o $(idtDirectory)/idt.o $(cpuDirectory)/paging.o
	ld $^ -o $@ -r
#############################
# 8259A
PICDirectory := io/8259A
$(PICDirectory)/8259A.o : $(PICDirectory)/8259A.c $(PICDirectory)/8259A.h
	gcc $< -o $@ $(CFLAGS)
#############################
# generate io.o
ioDirectory := io
$(ioDirectory)/io.o : $(portDirectory)/port.o $(screenDirectory)/screen.o $(diskDirectory)/disk.o $(cpuDirectory)/cpu.o $(PICDirectory)/8259A.o
	ld $^ -o $@ -r
###########################################################
# library
libraryDirectory := library
$(libraryDirectory)/library.o: $(libraryDirectory)/library.c $(libraryDirectory)/library.h
	gcc $< -o $@ $(CFLAGS)
###########################################################
# kernel
kernelDirectory := kernel
# jmp to main function in kernel
$(kernelDirectory)/jumpToKernelEntry.o: $(kernelDirectory)/jumpToKernelEntry.asm
	nasm $^ -o $@ -f elf
# kernel content
$(kernelDirectory)/kernel.o: $(kernelDirectory)/$(kernelDirectory).c
	gcc $^ -o $@ $(CFLAGS)
# jumpToKernelEntry + kernel
kernelORG := 0x1000 # like ORG in nasm
$(kernelDirectory)/kernel.bin: $(kernelDirectory)/jumpToKernelEntry.o $(kernelDirectory)/kernel.o $(libraryDirectory)/library.o $(ioDirectory)/io.o
	ld $^ -o $@ -Ttext $(kernelORG) --oformat binary
###########################################################
# remove "*.o" "*.bin"
clean:
	@echo "clean *.o and *.bin recursive"
	@find . -name "*.o" | xargs rm -f
	@find . -name "*.bin" | xargs rm -f