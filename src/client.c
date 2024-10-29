#include "../include/convert.h"
#include "../include/display.h"
#include "../include/networkutils.h"
#include "../include/open.h"
#include "../include/read.h"
#include "../include/write.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define FIFO_INPUT "./fifo/input"
// #define FIFO_OUTPUT "./fifo/output"

#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3
static in_port_t convert_port(const char *str, int *err);

int main(int argc, char *argv[])
{
    const char      *PORT;
    struct socketNet data;
    int              option;
    char             currentChar;
    int              err;

    int         retval     = EXIT_SUCCESS;
    const int   TOTAL_ARGS = 6;
    const char *message    = NULL;
    // const char *ip         = NULL;
    data.ip         = NULL;
    data.conversion = ' ';
    PORT            = "9999";
    err             = 0;
    if(argc != TOTAL_ARGS)
    {
        perror("Error: invalid number of arguments.");
        retval = EXIT_FAILURE;
        goto done;
    }

    while((option = getopt(argc, argv, "i:s:c:")) != -1)
    {
        switch(option)
        {
            case 'i':
                data.ip = optarg;
                break;
            case 's':
                message = optarg;
                break;
            case 'c':
                data.conversion = getConvertOption(optarg);
                break;
            default:
                perror("Error: invalid options.");
                retval = EXIT_FAILURE;
                goto done;
        }
    }
    if(data.ip == NULL || message == NULL || data.conversion == ' ')
    {
        perror("Error: error assigning option arguments.");
        retval = EXIT_FAILURE;
        goto done;
    }

    data.fd      = 0;
    data.inport  = convert_port(PORT, &err);
    data.outport = convert_port(PORT, &err);

    data.fd = open_network_socket_client(data.ip, data.outport, &err);
    if(data.fd == -1)
    {
        perror("Error: unable to open socket in client.");
        retval = EXIT_FAILURE;
        goto done;
    }

    // u192.168.0.XX
    // message
    // writeStr(fifoIn, ip);
    // writeChar(fifoIn, '\n');
    writeChar(data.fd, data.conversion);
    writeStr(data.fd, message);
    writeChar(data.fd, '\0');

    while((currentChar = readChar(data.fd)) != EOF)
    {
        if(currentChar == '\0')
        {
            break;
        }
        displayChar(currentChar);
    }

    display("\nclient ran successfully");

    // cleanup:
    close(data.fd);
done:
    return retval;
}

static in_port_t convert_port(const char *str, int *err)
{
    in_port_t port;
    char     *endptr;
    long      val;

    *err  = ERR_NONE;
    port  = 0;
    errno = 0;
    val   = strtol(str, &endptr, 10);    // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    // Check if no digits were found
    if(endptr == str)
    {
        *err = ERR_NO_DIGITS;
        goto done;
    }

    // Check for out-of-range errors
    if(val < 0 || val > UINT16_MAX)
    {
        *err = ERR_OUT_OF_RANGE;
        goto done;
    }

    // Check for trailing invalid characters
    if(*endptr != '\0')
    {
        *err = ERR_INVALID_CHARS;
        goto done;
    }

    port = (in_port_t)val;

done:
    return port;
}
