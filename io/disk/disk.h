#pragma once
void LBA_read_disk_sector(unsigned short *addressBegin, int start, unsigned char sectorNumbers);
void LBA_write_disk_sector(unsigned short *addressBegin, int start, unsigned char sectorNumbers);