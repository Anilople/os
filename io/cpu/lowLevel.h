#pragma once
unsigned short getCS();

/*
    https://wiki.osdev.org/Interrupt_Descriptor_Table

    Limit: 0~15
    Base: 16~47

    !!!! watch out the gap between Base and Limit
    in c programming language, expecial in struct,
    when Limit is a short, Base is an int,
    there is a "gap", i.e 16 bits 0 after Limit memory,
    because c programming language have memory align,
    for increse the efficiency.
*/
void LIDT(unsigned int *address);

void SIDT(unsigned int *address);

/*
    INT(255) -> {__asm__("INT $""255");} -> {__asm__("INT $255");}
*/
#define INT(number) {__asm__("INT $"#number);}