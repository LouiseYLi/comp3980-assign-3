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

int writeChar(int fd, const char c)
{
    ssize_t bytes_written = send(fd, &c, sizeof(char), 0);
    if(bytes_written == -1)
    {
        perror("Error: could not write fd.");
        return -1;
    }
    return 0;
}

ssize_t copy(size_t size, int *err, void *arg)
{
    char                   *buf;
    ssize_t                 retval;
    ssize_t                 nread;
    ssize_t                 nwrote;
    convertChar             convertFunction;
    const struct socketNet *data = (struct socketNet *)arg;
    convertFunction              = checkConvertArgs(data->conversion);
    *err                         = 0;
    errno                        = 0;
    buf                          = (char *)malloc(size);

    if(buf == NULL)
    {
        *err   = errno;
        retval = -1;
        goto done;
    }

    // Reads and converts message, then writes converted message
    do
    {
        errno = 0;
        nread = recv(data->client_fd, buf, size, 0);

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
            twrote    = send(data->client_fd, &buf[nwrote], remaining, 0);

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
done:
    return retval;
}
