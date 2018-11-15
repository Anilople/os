#include "paging.h"

void PTE_init(struct PTE *now, unsigned int *frameAddress)
{
    now->P  = 1;
    now->RW = 1;
    now->US = 1;
    now->PWT = 0;
    now->PCD = 0;
    now->A = 0;
    now->D = 0;
    now->PAT = 0;
    now->G = 0;
    now->AVL = 0;
    now->address_12_15 = ((int)frameAddress & 0x0000f000) >> 12;
    now->address_16_31 = ((int)frameAddress & 0xffff0000) >> 16;
}

void PTE_set(struct PTE *begin, int number, struct PTE *now)
{
    begin[number] = *now;
}

void PDE_init(struct PDE *now, unsigned int *pageTableAddress)
{
    now->P  = 1;
    now->RW = 1;
    now->US = 1;
    now->PWT = 0;
    now->PCD = 0;
    now->A = 0;
    now->D = 0;
    now->PAT = 0;
    now->G = 0;
    now->AVL = 0;
    now->address_12_15 = ((int)pageTableAddress & 0x0000f000) >> 12;
    now->address_16_31 = ((int)pageTableAddress & 0xffff0000) >> 16;
}

void PDE_set(struct PDE *begin, int number, struct PDE *now)
{
    begin[number] = *now;
}

// Page Directory Base Register, ie cr3
void PDBR_set(unsigned int *address)
{
    __asm__(
        "andl $0xfffff000, %%eax\n\t"
        "movl %%eax, %%cr3\n\t"
        : // output
        : "a"(address)
    );
}

/*
    PG is a bit in cr0 register(position 31)
    let PG = 1
*/
void openPaging()
{
    __asm__(
        "movl %%cr0, %%eax\n\t"
        "orl  $0x80000000, %%eax\n\t"
        "movl %%eax, %%cr0\n\t"
        :
        :
    );
}