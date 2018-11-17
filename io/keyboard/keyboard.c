/*
    there is a queue to save scancode,
    and another queue to save char which parsed from scancode queue.
*/

#define SCANCODE_BUFFER_SIZE 8
static unsigned char scancodeBuffer[SCANCODE_BUFFER_SIZE];
static unsigned int scancodeReadIndex = 0;
static unsigned int scancodeWriteIndex = 0;

static inline int scancodeBufferEmpty()
{
    return scancodeReadIndex == scancodeWriteIndex;
}

static inline int scancodeBufferFull()
{
    return scancodeReadIndex == (scancodeWriteIndex + 1) % sizeof(scancodeBuffer);
}

/*
    push succees return 1;
    push fail return 0;
*/
int pushKeyboardScancode(unsigned char code)
{
    if(!scancodeBufferFull()) {
        scancodeBuffer[scancodeWriteIndex] = code;

        scancodeWriteIndex++;
        scancodeWriteIndex %= sizeof(scancodeBuffer);

        return 1;
    } else {
        return 0;
    }
}

/*
    return 0 if pop fail(because may no data to pop)
*/
static inline unsigned char popKeyboardScancode()
{
    if(scancodeBufferEmpty()) {
        return 0;
    } else {
        scancodeReadIndex++;
        scancodeReadIndex %= sizeof(scancodeBuffer);
        return 1;
    }
}

/*
    return 0xff if no scancode data
*/
static inline unsigned char getKeyboardScancode()
{
    if(scancodeBufferEmpty()) {
        return 0xff;
    } else {
        return scancodeBuffer[scancodeReadIndex];
    }
}

/* some function about scancode */
static inline int isReleased(unsigned char scancode)
{
    return (scancode & 0b10000000) > 0;
}

static inline int isPressed(unsigned char scancode)
{
    return !isReleased(scancode);
}

static inline unsigned char toReleased(unsigned char scancode)
{
    return scancode | 0b10000000;
}

static inline unsigned char toPressed(unsigned char scancode)
{
    return scancode & 0b01111111;
}

/*
    "US QWERTY"

    just implement subset

    return '\0' if not success
*/
static inline char pressedScancodeToChar(unsigned char scancode)
{
    static const char *queryArray = 
        "\0\0""1234567890"      // 0x00 ~ 0x0B
        "-=\b\tQWERTYUI"        // 0x0C ~ 0x17
        "OP[]\n\0ASDFGH"        // 0x18 ~ 0x23
        "JKL;'`\0\\ZXCV"        // 0x24 ~ 0x2F
        "BNM,./\0\0\0 \0\0\0";  // 0x30 ~ 0x3B
    if(0x02 <= scancode && scancode <= 0x3B) { // 1 ~ /
        return queryArray[scancode];
    } else {
        return '\0';
    }
}

/* build an abstraction base of scancode */

#define CHAR_BUFFER_SIZE 8
static char charBuffer[CHAR_BUFFER_SIZE];
static unsigned int charReadIndex = 0;
static unsigned int charWriteIndex = 0;

static inline int charBufferEmpty()
{
    return charReadIndex == charWriteIndex;
}

static inline int charBufferFull()
{
    return charReadIndex == (charWriteIndex + 1) % sizeof(charBuffer);
}

/*
    return 1 if success.
    return 0 if failed.
*/
static inline int pushChar(char c)
{
    if(!charBufferFull()) {
        charBuffer[charWriteIndex] = c;

        charWriteIndex++;
        charWriteIndex %= sizeof(charBuffer);

        return 1;
    } else {
        return 0;
    }
}

/*
    return 0 if pop fail(because may no data to pop)
*/
static inline int popChar()
{
    if(charBufferEmpty()) {
        return 0;
    } else {
        charReadIndex++;
        charReadIndex %= sizeof(charBuffer);
        return 1;
    }
}

/*
    return 0 if no data in char buffer
*/
static inline char getCharFromCharBuffer()
{
    if(charBufferEmpty()) {
        return 0;
    } else {
        return charBuffer[charReadIndex];
    }
}

/*

*/
static inline void parseCharFromScancode()
{
    while(!scancodeBufferEmpty()) {
        unsigned char scancode = getKeyboardScancode();
        popKeyboardScancode();
        
        if(isPressed(scancode)) {
            char c = pressedScancodeToChar(scancode);
            if('\0' != c) {
                pushChar(c);
            } else { // transform failed

            }
        } else { // released

        }
    }
}

/*
    for program using;
    get input from keyboard;
*/
char getChar()
{
    for(parseCharFromScancode(); charBufferEmpty(); parseCharFromScancode()) {
        __asm__("hlt");
    }
    // to here, there must at least one char in charBuffer queue
    char c = getCharFromCharBuffer();
    popChar();
    return c;
}