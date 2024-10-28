#include "../include/write.h"

typedef char (*convertChar)(char);

struct clientData
{
    int  fifoIn;
    int  fifoOut;
    char conversion;
};

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

ssize_t copy(int fd_in, int fd_out, size_t size, int *err, void *arg)
{
    char                    *buf;
    ssize_t                  retval;
    ssize_t                  nread;
    ssize_t                  nwrote;
    convertChar              convertFunction;
    const struct clientData *data = (struct clientData *)arg;

    convertFunction = checkConvertArgs(data->conversion);
    *err            = 0;
    errno           = 0;
    buf             = (char *)malloc(size);

    if(buf == NULL)
    {
        *err   = errno;
        retval = -1;
        goto done;
    }

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

done:
    return retval;
}
