#include "descriptors.h"
#include "../screen/screen.h"
// void print_TABLE_Location(struct TABLE_Location *address)
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
    1 IDT use 64 bits
*/
void IDT_SET_GATE(unsigned int startAddress, unsigned char number, struct INTERRUPT_GATE* now)
{
    struct INTERRUPT_GATE *destinationAddress = ((struct INTERRUPT_GATE *) startAddress) + number;
    *destinationAddress = *now;
}

void print_INTERRUPT_GATE(struct INTERRUPT_GATE *address)
{
    int *testAddr = (int *) address;
    putString("IDT 0~15:0x");
    printUnsignedInt(*testAddr);
    putString("   ---   ");

    putString("IDT 16~31:0x");
    printUnsignedInt(*(testAddr + 1));
    putString("\r\n");
}

void initTable(struct INTERRUPT_GATE *table)
{
    struct INTERRUPT_GATE test;
    test.OFFSET_0_15 = 0x1000;
    test.SELECTOR = 0b00001000;
    test.ZERO = 0;
    test.TYPE = 0b1110;
    test.S = 0;
    test.DPL = 0;
    test.P = 1;
    test.OFFSET_16_31 = 0x0000;
    for(int i = 0; i < 256; i++) {
        table[i] = test;
    }
}