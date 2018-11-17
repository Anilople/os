#include "../../screen/screen.h"
#include "../../port/port.h"
/*
    do not use int
    watch out memory align
    especial 16 bits 0 after limit
*/
struct tableLocation {
    unsigned short LIMIT;
    unsigned short BASE_0_15;
    unsigned short BASE_16_31;
};

// void print_tableLocation(struct tableLocation *address)
// {
//     unsigned short *addressShort = (unsigned short *) address;
//     putString("0x");
//     printUnsignedShort(*addressShort);
//     putChar(' ');

//     addressShort++;
//     putString("0x");
//     printUnsignedShort(*addressShort);
//     putChar(' ');

//     addressShort++;
//     putString("0x");
//     printUnsignedShort(*addressShort);
//     putChar(' ');

//     addressShort++;
//     putString("0x");
//     printUnsignedShort(*addressShort);
//     putChar(' ');

//     putString("\r\n");      
// }


/*
<<INTEL 80386 PROGRAMMER'S REFERENCE MANUAL 1986>> from Page 157
https://wiki.osdev.org/Interrupt_Descriptor_Table
*/
struct INTERRUPT_GATE {
    unsigned short OFFSET_0_15;
    unsigned short SELECTOR;
    unsigned char ZERO; // 0 0 0 (NOT USED)
    unsigned char TYPE : 4; // fixed with 1 1 1 0
    unsigned char S : 1; // 0 for interrupt gate
    unsigned char DPL : 2;
    unsigned char P : 1;
    unsigned short OFFSET_16_31;
};

// static void print_INTERRUPT_GATE(struct INTERRUPT_GATE *address)
// {
//     int *testAddr = (int *) address;
//     putString("IDT 0~15:0x");
//     printUnsignedInt(*testAddr);
//     putString("   ---   ");

//     putString("IDT 16~31:0x");
//     printUnsignedInt(*(testAddr + 1));
//     putString("\r\n");
// }

static struct INTERRUPT_GATE INTERRUPT_GATE_get(unsigned short selector, unsigned int offset)
{
    struct INTERRUPT_GATE gate;
    gate.SELECTOR = selector;
    gate.OFFSET_0_15 = (unsigned short) (offset & 0x0000ffff); // low 16 bits
    gate.OFFSET_16_31 = (unsigned short) (offset >> 16); // high 16 bits

    // DPL, P fixed now
    gate.DPL = 0b00;
    gate.P = 1;

    // fixed part
    gate.ZERO = 0;
    gate.TYPE = 0b1110;
    gate.S = 0;
    return gate;
}

// emulate iret in c
#define EMULATE_IRET() __asm__("leave\n\t""iret\n\t"::)

// __attribute__((interrupt)) // it is not work
static void divideError()
{
    static char name[] = "divide error.\r\n";
    putString(name);

    EMULATE_IRET();
}

/*
    clock
*/
static void IRQ0()
{
    // putString("IRQ0 tick 0x");
    // static unsigned int count = 0;
    // printUnsignedInt(count);
    // count++;
    // putString("\r\n");
    port_byte_out(0x20, 0b00100000); // end interrupt
    
    EMULATE_IRET();
}

/*
    keyboard

    0x60 : read write buffer
    0x64 : get register status or send command
*/
static void IRQ1()
{
    unsigned char code = port_byte_in(0x60);
    putString("scan code:0x");
    printUnsignedChar(code);
    putString("\r\n");
    
    port_byte_out(0x20, 0b00100000); // end interrupt
    EMULATE_IRET();
}

/*
    install interrupt gate
*/
static inline void IDT_SET_GATE(struct INTERRUPT_GATE *base, unsigned short number, struct INTERRUPT_GATE *now)
{
    base[number] = *now;
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
static void LIDT(struct tableLocation *address)
{
    __asm__(
        "LIDT (%%eax)"
        : // output
        : "a"(address) // input
    );
}

// static void SIDT(struct tableLocation *address)
// {
//     __asm__(
//         "SIDT (%%eax)"
//         : // output
//         : "a"(address) // input
//     );
// }

void IDT_init(unsigned int *base, unsigned short limit)
{
    // set idt register
    struct tableLocation forLIDT;
    forLIDT.LIMIT = limit;
    forLIDT.BASE_0_15 = (unsigned short) (unsigned int) base;
    forLIDT.BASE_16_31 = (unsigned short) ((unsigned int)base >> 16);
    LIDT(&forLIDT);

    unsigned short gateSelector = 0b1000;
    // install interrupt gate
    struct INTERRUPT_GATE gate0 = INTERRUPT_GATE_get(gateSelector, (unsigned int) &divideError);
    IDT_SET_GATE((struct INTERRUPT_GATE *) base, 0, &gate0);

    struct INTERRUPT_GATE gateIRQ0 = INTERRUPT_GATE_get(gateSelector, (unsigned int) &IRQ0);
    IDT_SET_GATE((struct INTERRUPT_GATE *) base, 0x20, &gateIRQ0);

    struct INTERRUPT_GATE gateIRQ1 = INTERRUPT_GATE_get(gateSelector, (unsigned int) &IRQ1);
    IDT_SET_GATE((struct INTERRUPT_GATE *) base, 0x21, &gateIRQ1);
}