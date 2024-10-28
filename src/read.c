#include "../include/read.h"

char readChar(int fifo)
{
    char    c;
    ssize_t bytes_read = read(fifo, &c, sizeof(char));
    if(bytes_read == -1)
    {
        perror("Error: reading file.");
        return EOF;
    }
    if(bytes_read == 0)
    {
        return EOF;
    }
    if(c == '\0')
    {
        return c;
    }
    return c;
}
