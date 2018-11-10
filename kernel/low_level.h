unsigned short getCS()
{
    unsigned short CS;
    __asm__(
        "movw %%cs, %%ax"
        :"=a"(CS) // output
        :
    );
    return CS;
}

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
void LIDT(unsigned int *address)
{
    __asm__(
        "LIDT (%%eax)"
        : // output
        : "a"(address) // input
    );
}

void SIDT(unsigned int *address)
{
    __asm__(
        "SIDT (%%eax)"
        : // output
        : "a"(address) // input
    );
}

#define IRET()     {__asm__("IRET\n\t");}
#define INT0()     {__asm__("INT $0\n\t");}
#define INT255()   {__asm__("INT $255\n\t");}