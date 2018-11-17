#include "../io/io.h"
#include "../library/library.h"

// #define IDT_SATRT_ADDRESS ((unsigned int*) 0x10000)

// void INTERRUPT_GATE_INIT(struct INTERRUPT_GATE *address, unsigned int offset)
// {
//     struct INTERRUPT_GATE test;
//     address->OFFSET_0_15 = (offset & 0x0000ffff);
//     address->SELECTOR = 0b00001000;
//     address->ZERO = 0;
//     address->TYPE = 0b1110;
//     address->S = 0;
//     address->DPL = 0;
//     address->P = 1;
//     address->OFFSET_16_31 = (offset & 0xffff0000) >> 16;
// }

void outHello()
{
    putString("hello, world");
}

void pagingUsing()
{
    putString("function putString address:0x");
    printUnsignedInt((unsigned int)&putString);
    putString("\r\n");
    putString("function putChar address:0x");
    printUnsignedInt((unsigned int)&putChar);
    putString("\r\n");    
    // string to check
    char msgPaging[] = "wow, you open paging success.\r\n";
    void * pagingStringAddress = (void *)0x200000;
    memcpy(pagingStringAddress, (void*)msgPaging, sizeof(msgPaging));
    struct PDE *PDE_begin = (struct PDE *)0x100000;
    struct PTE *PTE_begin = (struct PTE *) (PDE_begin + 1024);
    // PDE 0
    struct PDE pde_zero;
    PDE_init(&pde_zero, (unsigned int *)PTE_begin);
    PDE_set(PDE_begin, 0, &pde_zero);
    // PTE 0
    struct PTE pte_one;
    PTE_init(&pte_one, (unsigned int*)0x1000);
    for(int i = 0; i < 1024; i++) {
        PTE_set(PTE_begin, i, &pte_one);
    }
    struct PTE pte_zero;
    PTE_init(&pte_zero, (unsigned int *)pagingStringAddress);
    PTE_set(PTE_begin, 0, &pte_zero);
    struct PTE pte_forVedioRam;
    PTE_init(&pte_forVedioRam, (unsigned int*)0xb8000);
    PTE_set(PTE_begin, 0xb8, &pte_forVedioRam);
    // 
    putString("now, set page directory base.\r\n");
    PDBR_set((unsigned int *)PDE_begin);
    putString("now, open paging.\r\n");
}

int main()
{
    // putString("there are 0x");
    // unsigned short memoryCanUse = *(unsigned short*)(0x500);
    // printUnsignedShort(memoryCanUse);
    // putString(" * 1KB can use.\r\n");

    // Paging
    // pagingUsing();
    // openPaging();
    // putString("now try pring msg in 0x200000.\r\n");
    // putString((char *) 0);
    // __asm__("hlt");
    
    _8259A_init();
    IDT_init((unsigned int*) 0x10000, 0xffff);
    INT(0);
    putString("here, here");

    putString("now, test keyboard input\r\n");
    __asm__("sti"); // enable interrupt
    while(1) {
        // __asm__("hlt");
        char c = getChar();
        if('\n' != c) {
            putChar(c);
        } else {
            putString("\r\n");
        }
    }
    // static unsigned int *count = ((int*) 0x50000);
    // putString("count = "); printUnsignedInt(*count); putString("\r\n");
    // if(*count > 0) {
    //     putString("have run this code. so hlt now");
    //     __asm__("hlt");
    // } else {
    //     (*count)++;
    // }

    // static struct INTERRUPT_GATE table[256];
    // // initTable(table);

    // struct TABLE_Location idt_table;
    // putString("idtr table address:0x");
    // printUnsignedInt((unsigned int)&idt_table);
    // putString("\r\n");
    // idt_table.LIMIT = 0xffff;
    // idt_table.BASE_0_15 = (int)table & 0x0000ffff;
    // idt_table.BASE_16_31 = ((int)table & 0xffff0000) >> 16;


    // LIDT((unsigned int*)&idt_table);

    // struct INTERRUPT_GATE test;
    // INTERRUPT_GATE_INIT(&test, (unsigned int) &outHello);
    // // *(unsigned short *) (0x100000) = 0x00cf; // this code mean iret
    // // print_INTERRUPT_GATE(&(table[0]));
    // IDT_SET_GATE(table, 255, &test);
    // INT(255);

    // putString("after use INT 255.\r\n");

    return 0;
}