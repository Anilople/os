#include "../drivers/ports.h"
#include "../drivers/disk.h"
#include "../screen/screen.h"
#include "../library/library.h"

int main()
{
    putString("hello\r\n");

    // unsigned short *base = (unsigned short *) 0x3000;
    // for(int i = 0; i < 256 * 4; i++) { // 4个扇区的数据
    //     if(0 == i % 40) {
    //         base[i] = 0x1234;
    //     } else {
    //         base[i] = ('b' << 8) + 'a';
    //     }
    // }
    
    // // CHS_read_one_disk_sector(base, 0, 0, 14, 3);
    // // LBA_read_one_disk_sector(base, 10);
    // // LBA_read_disk_sector(base, 13, 2);
    // putString("write them to disk.\r\n");
    // LBA_write_disk_sector(base, 20, 4);
    // putString("write finished.\r\n");
    return 0;
}