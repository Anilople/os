#include "low_level.h"
#include "descriptors.h"
#include "../drivers/ports.h"
#include "../drivers/disk.h"
#include "../screen/screen.h"
#include "../library/library.h"

#define IDT_SATRT_ADDRESS ((unsigned int*) 0x10000)

void INTERRUPT_GATE_INIT(struct INTERRUPT_GATE *address, unsigned int offset)
{
    struct INTERRUPT_GATE test;
    address->OFFSET_0_15 = (offset & 0x0000ffff);
    address->SELECTOR = 0b00001000;
    address->ZERO = 0;
    address->TYPE = 0b1110;
    address->S = 0;
    address->DPL = 0;
    address->P = 1;
    address->OFFSET_16_31 = (offset & 0xffff0000) >> 16;
}

int main()
{
    static unsigned int *count = ((int*) 0x50000);
    putString("count = "); printUnsignedInt(*count); putString("\r\n");
    if(*count > 0) {
        putString("have run this code. so hlt now");
        __asm__("hlt");
    } else {
        (*count)++;
    }

    static struct INTERRUPT_GATE table[256];
    // initTable(table);

    struct TABLE_Location idt_table;
    putString("idtr table address:0x");
    printUnsignedInt((unsigned int*)&idt_table);
    putString("\r\n");
    idt_table.LIMIT = 0xffff;
    idt_table.BASE_0_15 = (int)table & 0x0000ffff;
    idt_table.BASE_16_31 = ((int)table & 0xffff0000) >> 16;


    LIDT((unsigned int*)&idt_table);

    struct INTERRUPT_GATE test;
    INTERRUPT_GATE_INIT(&test, 0x1000);
    // print_INTERRUPT_GATE(&(table[0]));
    IDT_SET_GATE(table, 255, &test);
    INT255();

    return 0;
}