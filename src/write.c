#include "../include/write.h"

typedef char (*convertChar)(char);

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

int readUntilNewline(int fd, char *buf)
{
    int  retval = -1;
    int  index  = 0;
    char currentChar;
    currentChar = '\0';
    while(currentChar != '\n')
    {
        ssize_t nread = read(fd, &buf[index], sizeof(char));
        currentChar   = buf[index];
        if(nread <= 0)
        {
            retval = nread;
            display("Error: unable to find delimiter for ip.");
            goto done;
        }
    }
done:
    return retval;
}

ssize_t copy(int fd_in, int fd_out, size_t size, int *err)
{
    char       *buf;
    char       *ip;
    ssize_t     retval;
    ssize_t     nread;
    ssize_t     nwrote;
    convertChar convertFunction;
    char        conversion;
    *err  = 0;
    errno = 0;
    buf   = (char *)malloc(size);

    if(buf == NULL)
    {
        *err   = errno;
        retval = -1;
        goto done;
    }

    ip = (char *)malloc(size);
    if(ip == NULL)
    {
        *err   = errno;
        retval = -1;
        free(buf);
        goto done;
    }

    conversion = readChar(fd_in);
    if(conversion == EOF)
    {
        *err   = errno;
        retval = -1;
        goto cleanup;
    }

    readUntilNewline(fd_in, ip);

    convertFunction = checkConvertArgs(conversion);

    // Reads and converts message, then writes converted message
    do
    {
        errno = 0;
        nread = read(fd_in, buf, size);

        if(nread < 0)
        {
            *err   = errno;
            retval = -2;
            goto cleanup;
        }

        convertBuffer(buf, convertFunction);

        nwrote = 0;

        do
        {
            ssize_t twrote;
            size_t  remaining;

            remaining = (size_t)(nread - nwrote);
            errno     = 0;
            twrote    = write(fd_out, &buf[nwrote], remaining);

            if(twrote < 0)
            {
                *err   = errno;
                retval = -3;
                goto cleanup;
            }

            nwrote += twrote;
        } while(nwrote != nread);
    } while(nread != 0);

    retval = nwrote;
cleanup:
    free(buf);
    free(ip);
done:
    return retval;
}
