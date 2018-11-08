/*
    infomation from 《x86汇编语言:从实模式到保护模式》, page 154, 光标
*/
unsigned short getCursor();
void setCursor(unsigned short position);
void putChar(char value);

/*----------------------------------------------*/
/*
    the functions below this line, is an abstraction base on function above
*/
void putString(char *string);
void print4bits(char value);
void printUnsignedChar(unsigned char value);
void printUnsignedShort(unsigned short value);
void printUnsignedInt(unsigned int value);