#include "../include/read.h"

char readChar(int fd)
{
    char    c;
    ssize_t bytes_read;

    do
    {
        bytes_read = recv(fd, &c, sizeof(char), 0);
    } while(bytes_read == -1 && errno == EINTR);    // Retry on interrupted system calls

    if(bytes_read <= 0)
    {
        return EOF;    // Handle errors or end-of-stream
    }

    return c;
}

// char readChar(int fd)
// {
//     char    c;
//     ssize_t bytes_read = recv(fd, &c, sizeof(char));
//     if(bytes_read == -1)
//     {
//         perror("Error: reading file.");
//         return EOF;
//     }
//     if(bytes_read == 0)
//     {
//         return EOF;
//     }
//     if(c == '\0')
//     {
//         return c;
//     }
//     return c;
// }
