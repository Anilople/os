/*
    <<操作系统真相还原>> Page 195
*/
#pragma once
/*
    Page Table
        Page Table Entry(PTE, 4 Bytes)
        Page Table Entry(PTE, 4 Bytes)
        Page Table Entry(PTE, 4 Bytes)
        ...
*/
struct PTE {
    // 0~7
    unsigned char P   : 1; // present; 1 mean exist, 0 mean not exist
    unsigned char RW  : 1; // 1 mean read and write; 0 mean read and no write
    unsigned char US  : 1; // 1 mean user, 0 mean supervisor(0, 1, 2 privileage can use)
    unsigned char PWT : 1; // 0
    unsigned char PCD : 1; // 0
    unsigned char A   : 1; // 
    unsigned char D   : 1; // Dirty
    unsigned char PAT : 1; // 0
    // 8~11
    unsigned char G   : 1; // 1 for TLB, 0 not
    unsigned char AVL : 3; // available, we not use
    // 12~31 follow
    //     12~15
    unsigned char address_12_15 : 4;
    //     16~31
    unsigned short address_16_31;
};

void PTE_init(struct PTE *now, unsigned int *frameAddress);

void PTE_set(struct PTE *begin, int number, struct PTE *now);

/*
    Page Directory Entry(PDE)
    through PDE to find PTE
*/
struct PDE {
    // 0~7
    unsigned char P   : 1; // present; 1 mean exist, 0 mean not exist
    unsigned char RW  : 1; // 1 mean read and write; 0 mean read and no write
    unsigned char US  : 1; // 1 mean user, 0 mean supervisor(0, 1, 2 privileage can use)
    unsigned char PWT : 1; // 0
    unsigned char PCD : 1; // 0
    unsigned char A   : 1; // 
    unsigned char D   : 1; // Dirty, not change in PDE
    unsigned char PAT : 1; // 0
    // 8~11
    unsigned char G   : 1; // 1 for TLB, 0 not
    unsigned char AVL : 3; // available, we not use
    // 12~31 follow
    //     12~15
    unsigned char address_12_15 : 4;
    //     16~31
    unsigned short address_16_31;
};

void PDE_init(struct PDE *now, unsigned int *pageTableAddress);

void PDE_set(struct PDE *begin, int number, struct PDE *now);

// Page Directory Base Register, ie cr3
void PDBR_set(unsigned int *address);

/*
    PG is a bit in cr0 register(position 31)
    let PG = 1

    can not use function here
*/
#define openPaging() \
    __asm__( \
        "movl %%cr0, %%eax\n\t" \
        "orl  $0x80000000, %%eax\n\t" \
        "movl %%eax, %%cr0\n\t" \
        : \
        : \
        : "eax" \
    )

