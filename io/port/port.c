// in al, dx
unsigned char port_byte_in(unsigned short port)
{
    unsigned char result;
    __asm__(
        "in %%dx, %%al"
        : "=a"(result)
        : "d"(port)
    );
    return result;
}

// out dx, al
void port_byte_out(unsigned short port, unsigned char data)
{
    __asm__(
        "out %%al, %%dx"
        : // no output 
        : "d"(port), // edx = port
          "a"(data) // eax = data
    );
}

// in ax, dx
unsigned short port_word_in(unsigned short port)
{
    unsigned short result;
    __asm__(
        "in %%dx, %%ax"
        : "=a"(result) // result = eax
        : "d"(port) // edx = port
    );
    return result;
}

// out dx, ax
void port_word_out(unsigned short port, unsigned short data)
{
    __asm__(
        "out %%ax, %%dx"
        : // no output 
        : "d"(port), // edx = port
          "a"(data) // eax = data
    );
}