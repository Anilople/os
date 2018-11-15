#pragma once
// in al, dx
unsigned char port_byte_in(unsigned short port);

// out dx, al
void port_byte_out(unsigned short port, unsigned char data);

// in ax, dx
unsigned short port_word_in(unsigned short port);

// out dx, ax
void port_word_out(unsigned short port, unsigned short data);