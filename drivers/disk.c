#include "ports.h"
#include "../screen/screen.h"
/*
https://wiki.osdev.org/ATA_read/write_sectors
;=============================================================================
; ATA read sectors (CHS mode) 
; Max head index is 15, giving 16 possible heads
; Max cylinder index can be a very large number (up to 65535)
; Sector is usually always 1-63, sector 0 reserved, max 255 sectors/track
; If using 63 sectors/track, max disk size = 31.5GB
; If using 255 sectors/track, max disk size = 127.5GB
; See OSDev forum links in bottom of [http://wiki.osdev.org/ATA]
;
; @param EBX The CHS values; 2 bytes, 1 byte (BH), 1 byte (BL) accordingly
; @param CH The number of sectors to read
; @param RDI The address of buffer to put data obtained from disk               
;
; @return None
;=============================================================================
*/
void CHS_read_one_disk_sector(unsigned short *addressBegin, unsigned short cylinder, unsigned char head, unsigned char sector, unsigned char sectorCount)
{
    // send head numbers
    head &= 0b1111; // 0~15
    head |= 0b10100000; // default 1010b in high nibble
    port_byte_out(0x1f6, head);

    // sector count, read sectorCount sectors
    port_byte_out(0x1f2, sectorCount);
    // sector number(sector index)
    port_byte_out(0x1f3, sector);

    // cylinder low port
    port_byte_out(0x1f4, cylinder & 0x00ff);
    // cylinder high port
    port_byte_out(0x1f5, (cylinder & 0xff00) >> 8);

    // command port
    port_byte_out(0x1f7, 0x20); // read with retry

    unsigned char status = port_byte_in(0x1f7);
    // putString("status: 0x");
    // printUnsignedChar(status);
    // putString("\r\n");
    while(0 == (status & 0b00001000)) {
        status = port_byte_in(0x1f7);
        // putString("status: 0x");
        // printUnsignedChar(status);
        // putString("\r\n");
    }

    // read 256 words = 1 sector
    int dataSize = 256 * sectorCount;
    // putString("datasize: 0x");
    // printUnsignedInt(dataSize);
    // putString("\r\n");
    
    for(int i = 0; i < dataSize; i++) {
        unsigned short data = port_word_in(0x1f0);
        // if(0x01ff != data) {
        //     putString("0x");
        //     printUnsignedShort(data);
        //     putString("\r\n");
        // } else {
        //     putChar('X');
        // }
        addressBegin[i] = data;
    }
    // putString("\r\n");
}

/*
LBA28模式读取硬盘内容
addressBegin -- 读入内容的起始地址
LBAstart -- LBA模式起始扇区号
*/
void LBA_read_one_disk_sector(unsigned short *addressBegin, int LBAstart)
{
    // 给端口 0x1f2 要读取的扇区数量
    port_byte_out(0x1f2, 1); // 1个扇区

    // 设置起始LBA扇区号
    // 先粗暴地只用低8bits
    // 0~7
    port_byte_out(0x1f3, LBAstart & 0xff);
    // 8~15
    port_byte_out(0x1f4, 0);
    // 16~23
    port_byte_out(0x1f5, 0);
    // 24~28
    port_byte_out(0x1f6, 0b11100000);

    // 向端口0x1f7写入0x20, 请求硬盘读
    port_byte_out(0x1f7, 0x20);

    // 等待硬盘就绪
    // putString("wait disk ready.\r\n");
    // while(1) {
    //     unsigned char diskStatus = port_byte_in(0x1f7); // 获取硬盘状态
    //     unsigned char bit7 = (diskStatus >> 7); // busy
    //     unsigned char bit3 = (diskStatus & 0b00001000) >> 3; // ready
    //     unsigned char bit0 = (diskStatus & 0b1); // error
    //     if(bit0 > 0) {
    //         putString("there is error.\r\n");
    //     } else{
    //         if(bit7 > 0) {
    //             putString("disk is busy.\r\n");
    //         } else {

    //         }
    //         if(bit3 > 0) {
    //             putString("disk is ready.\r\n");
    //             break;
    //         } else {

    //         }
    //     }
    // }

    // 开始从硬盘中拿数据, 从0x1f0端口中, 每次取出 16 bits
    unsigned short *addressNow = addressBegin;
    for(int i = 0; i < 256; i++) { // 
        *addressNow = port_word_in(0x1f0);
        addressNow++;
    }
}

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
    unsigned short dataFromDisk;
    for(int i = 0; i < dataNumbers; i++) {
        port_word_out(0x1f0, addressBegin[i]);
    }
}