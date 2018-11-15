
/*
    suppose destination address < source address
*/
void memcpy(void *destination, const void *source, int size)
{
    char *destinationNow = (char *) destination;
    char *sourceNow = (char *) source;
    while(size > 0) {
        *destinationNow = *sourceNow;
        destinationNow++;
        sourceNow++;
        size--;
    }
}

void memset(void *addressBegin, int value, int size)
{
    unsigned char *addressNow = addressBegin;
    while(size > 0) {
        *addressNow = (unsigned char) value;
        addressNow++;
        size--;
    }
}