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

int main(int argc, char *argv[])
{
    struct socketNet data;
    int              option;
    char             currentChar;
    int              retval;
    int              err;

    const int   TOTAL_ARGS = 7;
    const char *PORT       = "9999";
    const char *MESSAGE    = NULL;
    retval                 = 0;
    err                    = 0;
    data.ip                = NULL;
    data.conversion        = ' ';
    data.server_fd         = 0;
    data.client_fd         = 0;
    data.inport            = convert_port(PORT, &err);
    data.outport           = convert_port(PORT, &err);

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
                MESSAGE = optarg;
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
    if(data.ip == NULL || MESSAGE == NULL || data.conversion == ' ')
    {
        perror("Error: error assigning option arguments.");
        retval = EXIT_FAILURE;
        goto done;
    }

    data.client_fd = open_network_socket_client(data.ip, data.outport, &err);
    if(data.client_fd == -1)
    {
        perror("Error: unable to open client socket.");
        retval = EXIT_FAILURE;
        goto done;
    }

    // umessage
    if(writeChar(data.client_fd, data.conversion) == -1)
    {
        retval = -1;
        goto cleanup;
    }
    if(writeStr(data.client_fd, MESSAGE) == -1)
    {
        retval = -1;
        goto cleanup;
    }
    if(writeChar(data.client_fd, '\0') == -1)
    {
        retval = -1;
        goto cleanup;
    }

    while((currentChar = readChar(data.client_fd)) != EOF)
    {
        if(currentChar == '\0')
        {
            break;
        }
        displayChar(currentChar);
    }
    display("\nclient ran successfully");
cleanup:
    close(data.client_fd);
done:
    return retval;
}
