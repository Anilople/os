#include "../port/port.h"
#include "../screen/screen.h"
/*
    <<操作系统真象还原>> Page 314
    Chapter 7.5
*/

/*
    0x20 for master
    0xA0 for slave
*/
#define PIC_MASTER_CONTROL  0x20
#define PIC_MASTER_DATA     0x21
#define PIC_SLAVE_CONTROL   0xA0
#define PIC_SLAVE_DATA      0xA1


struct _8259A_ICW1 {
    unsigned char IC4  : 1; // 1 mean you need to write ICW4 later, 0 mean not.
    unsigned char SNGL : 1; // 1 mean single; 0 mean not single
    unsigned char ADI  : 1; // for 8085, x86 do not need
    unsigned char LTIM : 1; // 0 表示边沿触发, 1 表示电平触发
    unsigned char rest : 4; // must be 0b0001
};

struct _8259A_ICW2 {
    unsigned char ID_0_2 : 3;
    unsigned char T_3_7 : 4; // where interrupt number begin
};

// no ICW3 because it is easy to set

struct _8259A_ICW4 {
    unsigned char uPM  : 1; // 1 for x86, 0 for 8080 or 8085
    unsigned char AEOI : 1; // 1 mean auto end of interrupt, 0 mean mannual
    unsigned char M_S  : 1; // 
    unsigned char BUF  : 1; // work in buffer or not
    unsigned char SFNM : 1; // 
    unsigned char zero : 3; // simply to 0, no function
};

static inline void _8259A_ICW1_set(struct _8259A_ICW1 *icw1, int master)
{
    if(master) {
        port_byte_out(PIC_MASTER_CONTROL, *(unsigned char*)icw1);
    } else {
        port_byte_out(PIC_SLAVE_CONTROL, *(unsigned char*)icw1);
    }
}

static inline void _8259A_ICW2_set(struct _8259A_ICW2 *icw2, int master)
{
    if(master) {
        port_byte_out(PIC_MASTER_DATA, *(unsigned char*)icw2);
    } else {
        port_byte_out(PIC_SLAVE_DATA, *(unsigned char*)icw2);
    }
}

static inline void _8259A_ICW3_set(unsigned char icw3, int master)
{
    if(master) {
        port_byte_out(PIC_MASTER_DATA, icw3);
    } else {
        port_byte_out(PIC_SLAVE_DATA, icw3);
    }
}

static inline void _8259A_ICW4_set(struct _8259A_ICW4 *icw4, int master)
{
    if(master) {
        port_byte_out(PIC_MASTER_DATA, *(unsigned char*)icw4);
    } else {
        port_byte_out(PIC_SLAVE_DATA, *(unsigned char*)icw4);
    }
}

static inline void _8259A_init_ICW()
{
    // master
    //      icw1
    struct _8259A_ICW1 masterICW1;
    masterICW1.IC4 = 1;
    masterICW1.SNGL = 0;
    masterICW1.ADI = 0;
    masterICW1.LTIM = 0;
    masterICW1.rest = 0b0001;
    _8259A_ICW1_set(&masterICW1, 1);
    //      icw2
    struct _8259A_ICW2 masterICW2;
    masterICW2.ID_0_2 = 0;
    masterICW2.T_3_7 = 0b100; // real start is 0b100 * 8 = 0b100000 = 0x20
    _8259A_ICW2_set(&masterICW2, 1);
    //      icw3
    _8259A_ICW3_set(0b00000100, 1); // IR2 have slave 8259
    //      icw4
    struct _8259A_ICW4 masterICW4;
    masterICW4.uPM = 1; // x86
    masterICW4.AEOI = 0;
    masterICW4.M_S = 0;
    masterICW4.BUF = 0;
    masterICW4.SFNM = 0;
    masterICW4.zero = 0b000;
    _8259A_ICW4_set(&masterICW4, 1);


    // slave
    //      icw1
    struct _8259A_ICW1 slaveICW1;
    slaveICW1.IC4 = 1;
    slaveICW1.SNGL = 0;
    slaveICW1.ADI = 0;
    slaveICW1.LTIM = 0;
    slaveICW1.rest = 0b0001;
    _8259A_ICW1_set(&slaveICW1, 0);
    //      icw2
    struct _8259A_ICW2 slaveICW2;
    slaveICW2.ID_0_2 = 0;
    slaveICW2.T_3_7 = 0b101; // real start is 0b101 * 8 = 0b101000 = 0x28
    _8259A_ICW2_set(&slaveICW2, 0);
    //      icw3
    _8259A_ICW3_set(0b00000010, 0); // connect to master IR2
    //      icw4
    struct _8259A_ICW4 slaveICW4;
    slaveICW4.uPM = 1; // x86
    slaveICW4.AEOI = 0;
    slaveICW4.M_S = 0;
    slaveICW4.BUF = 0;
    slaveICW4.SFNM = 0;
    slaveICW4.zero = 0b000;
    _8259A_ICW4_set(&slaveICW4, 0);
}

void _8259A_init()
{
    _8259A_init_ICW();
    putString("ICW1~4 finished.\r\n");
    //  OCW1
    putString("just open IR0.\r\n");
    port_byte_out(PIC_MASTER_DATA, 0b11111100); // just open IR0 in master
    port_byte_out(PIC_SLAVE_DATA, 0xff); // close all IR in slave
}