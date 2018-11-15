#pragma once
/*
    do not use int
    watch out memory align
    especial 16 bits 0 after limit
*/
struct TABLE_Location {
    unsigned short LIMIT;
    unsigned short BASE_0_15;
    unsigned short BASE_16_31;
};

// void print_TABLE_Location(struct TABLE_Location *address);

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

/*
    1 IDT use 64 bits
*/
void IDT_SET_GATE(unsigned int startAddress, unsigned char number, struct INTERRUPT_GATE* now);

void print_INTERRUPT_GATE(struct INTERRUPT_GATE *address);

void initTable(struct INTERRUPT_GATE *table);