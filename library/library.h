#pragma once
/*
    suppose destination address < source address
*/
void memcpy(void *destination, const void *source, int size);

void memset(void *addressBegin, int value, int size);