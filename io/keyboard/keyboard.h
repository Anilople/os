#pragma once
/*
    for interrupt using
    to save scancode;

    push succees return 1;
    push fail return 0;
*/
int pushKeyboardScancode(unsigned char code);

/*
    for program using;
    get input from keyboard;
*/
char getChar();