void CHS_read_one_disk_sector(
    unsigned short *addressBegin, 
    unsigned short cylinder,
    unsigned char head, 
    unsigned char sector,
    unsigned char sectorCount
    );
void LBA_read_one_disk_sector(unsigned short *addressBegin, int LBAstart);
void LBA_read_disk_sector(unsigned short *addressBegin, int start, unsigned char sectorNumbers);
void LBA_write_disk_sector(unsigned short *addressBegin, int start, unsigned char sectorNumbers);