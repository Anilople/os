#include "../drivers/ports.h"
#include "../library/library.h"

#define CURSORROW       25 // 25 rows in a screen
#define CURSORCOLUMN    80 // 80 columns in a screen
#define CURSORRANGE     (CURSORROW * CURSORCOLUMN) // cursor range
#define VIDEORAM        ((unsigned char *)0xb8000)

// char attribute -- foreground
#define FORE_I          0b00001000 // I = 1 mean deep; I = 0 mean light
#define FORE_RED        0b00000100
#define FORE_GREED      0b00000010
#define FORE_BLUE       0b00000001

// char attribute -- background
#define BACK_K          0b10000000 // K = 1 mean twinkle, 0 mean not twinkle
#define BACK_RED        0b01000000
#define BACK_GREED      0b00100000
#define BACK_BLUE       0b00010000

// conbine char attribute
#define FORE_WHITE    (FORE_RED | FORE_GREED | FORE_BLUE) // for normal while char using
#define BACK_TWINKLE    BACK_K // for cursor using

/*
    get position of cursor
*/
unsigned short getCursor()
{
    port_byte_out(0x3d4, 0x0e);
    unsigned short cursorHigh8 = port_byte_in(0x3d5);
    port_byte_out(0x3d4, 0x0f);
    unsigned short cursorLow8 = port_byte_in(0x3d5);
    return (cursorHigh8 << 8) | cursorLow8;
}

/*
    change position of cursor
*/
void setCursor(unsigned short position)
{
    // set high 8 bit
    port_byte_out(0x3d4, 0x0e);
    port_byte_out(0x3d5, position >> 8);
    // set low 8 bit
    port_byte_out(0x3d4, 0x0f);
    port_byte_out(0x3d5, position & 0x00ff);
}

/*
    compute cursor position when cursor simplely down to next row
*/
inline static unsigned short getNextRowCursor(unsigned short position)
{
    return position + CURSORCOLUMN;
}

/*
    compute cursor position when cursor simplely left move to head in this row
*/
inline static unsigned short getNowRowHeadCursor(unsigned short position)
{
    return position - (position % 80);
}

inline static void setCharValue(int index, char it)
{
    *(char *) (VIDEORAM + index + index) = it;
}

inline static void setCharAttribute(int index, unsigned char attribute)
{
    *(unsigned char *) (VIDEORAM + index + index + 1) = attribute;
}

inline static void scrollScreen()
{
    // row up
    memcpy(VIDEORAM, VIDEORAM + CURSORCOLUMN * 2, (CURSORRANGE - CURSORCOLUMN) * 2);
    // clean last row
    for(int lastRowStart = CURSORRANGE - CURSORCOLUMN;
        lastRowStart < CURSORRANGE;
        lastRowStart++) {
        setCharValue(lastRowStart, '\0');
        setCharAttribute(lastRowStart, FORE_WHITE);
    }
}

void putChar(char value)
{
    unsigned short cursorPosition = getCursor();
    unsigned short nextCursorPosition = 0;
    if('\n' == value) { // to next line
        nextCursorPosition = getNextRowCursor(cursorPosition);
    } else if('\r' == value) { // to line head
        nextCursorPosition = getNowRowHeadCursor(cursorPosition);
    } else { // normal char
        setCharValue(cursorPosition, value);
        setCharAttribute(cursorPosition, FORE_WHITE);
        nextCursorPosition = cursorPosition + 1; // increse cursor position
    }

    // judge if nextCursorPosition out of screen
    if(nextCursorPosition < CURSORRANGE) { // no scroll screen

    } else { // must scroll screen
        scrollScreen();
        // change cursor
        nextCursorPosition -= CURSORCOLUMN;
    }
    // set new cursor position
    setCursor(nextCursorPosition);
    // let next char position twinkle
}


/*----------------------------------*/

void putString(char *string)
{
    while('\0' != *string) {
        putChar(*string);
        string++;
    }
}

void print4bits(char value)
{
    value &= 0x0f;
    if(value < 10) {
        putChar(value + '0');
    } else {
        putChar(value - 10 + 'a');
    }
}

void printUnsignedChar(unsigned char value)
{
    print4bits((value & 0xf0) >> 4);
    print4bits((value & 0x0f) >> 0);
}

void printUnsignedShort(unsigned short value)
{
    print4bits((value & 0xf000) >> 12);
    print4bits((value & 0x0f00) >> 8);
    print4bits((value & 0x00f0) >> 4);
    print4bits((value & 0x000f) >> 0);
}

void printUnsignedInt(unsigned int value)
{
    print4bits((value & 0xf0000000) >> 28);
    print4bits((value & 0x0f000000) >> 24);
    print4bits((value & 0x00f00000) >> 20);
    print4bits((value & 0x000f0000) >> 16);

    print4bits((value & 0xf000) >> 12);
    print4bits((value & 0x0f00) >> 8);
    print4bits((value & 0x00f0) >> 4);
    print4bits((value & 0x000f) >> 0);
}