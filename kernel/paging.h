/*
    <<操作系统真相还原>> Page 195
*/


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