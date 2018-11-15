#include "../port/port.h"
#include "../screen/screen.h"
/*
https://wiki.osdev.org/ATA_read/write_sectors
*/

/*
LBA28模式读取硬盘内容
addressBegin -- 读入内容的起始地址
start -- 起始扇区号
sectorNumbers -- 要读入的扇区数量, 每次最多读256个,
                    为0的时候, 代表读入256个

*/
void LBA_read_disk_sector(unsigned short *addressBegin, int start, unsigned char sectorNumbers)
{
    // 给端口 0x1f2 要读取的扇区数量
    port_byte_out(0x1f2, sectorNumbers);

    // 设置起始LBA扇区号
    start &= 0xfffffff; // 保留低28位
    port_byte_out(0x1f2, sectorNumbers);
    // 0~7
    port_byte_out(0x1f3, start & 0xff);
    // 8~15
    port_byte_out(0x1f4, (start >> 8) & 0xff);
    // 16~23
    port_byte_out(0x1f5, (start >> 16) & 0xff);
    // 24~28
    unsigned char for_0x1f6_port = (start >> 24) & 0x0f;
    for_0x1f6_port |= 0b11100000; // 在高4bit放入读的是LBA模式, 并且用主硬盘等信息
    port_byte_out(0x1f6, for_0x1f6_port);

    // 向端口0x1f7写入0x20, 请求硬盘读
    port_byte_out(0x1f7, 0x20);

    // 等待硬盘就绪
    unsigned char diskStatus = port_byte_in(0x1f7); // 获取硬盘状态
    while(0b00001000 != (diskStatus & 0b10001000)) {
        diskStatus = port_byte_in(0x1f7); // 获取硬盘状态
    }

    // 开始从硬盘中拿数据, 从0x1f0端口中, 每次取出 16 bits
    int dataNumbers = sectorNumbers * 256; // 总共要读入多少次 16 bits
    unsigned short dataFromDisk;
    for(int i = 0; i < dataNumbers; i++) {
        dataFromDisk = port_word_in(0x1f0);
        addressBegin[i] = dataFromDisk;
    }
}


/*
LBA28模式写入硬盘内容
addressBegin -- 写入内容的起始地址
start -- 起始扇区号
sectorNumbers -- 要写入的扇区数量, 每次最多写入256个,
                    为0的时候, 代表写入256个

*/
void LBA_write_disk_sector(unsigned short *addressBegin, int start, unsigned char sectorNumbers)
{
    // 给端口 0x1f2 要写入的扇区数量
    port_byte_out(0x1f2, sectorNumbers);

    // 设置起始LBA扇区号
    start &= 0xfffffff; // 保留低28位
    port_byte_out(0x1f2, sectorNumbers);
    // 0~7
    port_byte_out(0x1f3, start & 0xff);
    // 8~15
    port_byte_out(0x1f4, (start >> 8) & 0xff);
    // 16~23
    port_byte_out(0x1f5, (start >> 16) & 0xff);
    // 24~28
    unsigned char for_0x1f6_port = (start >> 24) & 0x0f;
    for_0x1f6_port |= 0b11100000; // 在高4bit放入写入的是LBA模式, 并且用主硬盘等信息
    port_byte_out(0x1f6, for_0x1f6_port);

    // https://wiki.osdev.org/ATA_read/write_sectors
    // 向端口0x1f7写入0x30, 请求写入硬盘
    port_byte_out(0x1f7, 0x30);

    // 等待硬盘就绪
    unsigned char diskStatus = port_byte_in(0x1f7); // 获取硬盘状态
    while(0b00001000 != (diskStatus & 0b10001000)) {
        diskStatus = port_byte_in(0x1f7); // 获取硬盘状态
    }

    // 开始将数据写入硬盘, 从0x1f0端口中, 每次写入 16 bits
    int dataNumbers = sectorNumbers * 256; // 总共要写入多少次 16 bits
    
    for(int i = 0; i < dataNumbers; i++) {
        port_word_out(0x1f0, addressBegin[i]);
    }
}