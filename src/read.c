#include "../include/read.h"

char readChar(int fd)
{
    char    c;
    ssize_t bytes_read;

    do
    {
        bytes_read = read(fd, &c, sizeof(char));
    } while(bytes_read == -1 && errno == EINTR);    // Retry on interrupted system calls

    if(bytes_read <= 0)
    {
        return EOF;    // Handle errors or end-of-stream
    }

    return c;
}
