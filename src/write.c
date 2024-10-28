#include "../include/write.h"

int writeStr(int fifo, const char *buf)
{
    int i = 0;
    while(buf[i] != '\0')
    {
        if(writeChar(fifo, buf[i]) == -1)
        {
            return -1;
        }
        i++;
    }
    return 0;
}

int writeChar(int fifo, const char c)
{
    ssize_t bytes_written = write(fifo, &c, sizeof(char));
    if(bytes_written == -1)
    {
        perror("Error: could not write fifo.");
        return -1;
    }
    return 0;
}
