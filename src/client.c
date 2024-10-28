#include "../include/convert.h"
#include "../include/display.h"
#include "../include/read.h"
#include "../include/write.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FIFO_INPUT "./fifo/input"
#define FIFO_OUTPUT "./fifo/output"

int main(int argc, char *argv[])
{
    int  option;
    int  fifoIn;
    int  fifoOut;
    char currentChar;

    int         retval     = EXIT_SUCCESS;
    const int   TOTAL_ARGS = 5;
    const char *message    = NULL;
    char        conversion = ' ';
    if(argc != TOTAL_ARGS)
    {
        perror("Error: invalid number of arguments.");
        retval = EXIT_FAILURE;
        goto done;
    }

    while((option = getopt(argc, argv, "s:c:")) != -1)
    {
        switch(option)
        {
            case 's':
                message = optarg;
                break;
            case 'c':
                conversion = getConvertOption(optarg);
                break;
            default:
                perror("Error: invalid options.");
                retval = EXIT_FAILURE;
                goto done;
        }
    }
    if(message == NULL || conversion == ' ')
    {
        perror("Error: error assigning arguments.");
        retval = EXIT_FAILURE;
        goto done;
    }

    fifoIn = open(FIFO_INPUT, O_RDWR | O_CLOEXEC);
    if(fifoIn == -1)
    {
        perror("Error: unable to open input fifo in client.");
        retval = EXIT_FAILURE;
        goto done;
    }
    fifoOut = open(FIFO_OUTPUT, O_RDWR | O_CLOEXEC);
    if(fifoOut == -1)
    {
        perror("Error: unable to open output fifo in client.");
        close(fifoIn);
        retval = EXIT_FAILURE;
        goto done;
    }

    writeChar(fifoIn, conversion);
    writeStr(fifoIn, message);
    writeChar(fifoIn, '\0');

    while((currentChar = readChar(fifoOut)) != EOF)
    {
        if(currentChar == '\0')
        {
            break;
        }
        displayChar(currentChar);
    }

    display("\nclient ran successfully");

    close(fifoIn);
    close(fifoOut);
done:
    return retval;
}
