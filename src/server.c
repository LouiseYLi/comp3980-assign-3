#include "../include/convert.h"
#include "../include/display.h"
#include "../include/read.h"
#include "../include/write.h"
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FIFO_INPUT "./fifo/input"
#define FIFO_OUTPUT "./fifo/output"

typedef char (*convertChar)(char);

static void *handleClientRequest(void *arg);
void         handleSignal(int signal);

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;

struct clientData
{
    int  fifoIn;
    int  fifoOut;
    char conversion;
};

void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        terminate = 1;
    }
}

static void *handleClientRequest(void *arg)
{
    const struct clientData *data = (struct clientData *)arg;
    convertChar              convertFunction;
    char                     currentChar;
    convertFunction = checkConvertArgs(data->conversion);
    if(convertFunction == NULL)
    {
        perror("Error: obtaining specific convert function");
        return NULL;
    }
    while((currentChar = readChar(data->fifoIn)) != EOF)
    {
        if(writeChar(data->fifoOut, convertFunction(currentChar)) == -1)
        {
            perror("Error: error writing to fifo.");
            return NULL;
        }
        if(currentChar == '\0')
        {
            break;
        }
    }
    return NULL;
}

int main(void)
{
    struct clientData data;
    int               fifoIn;
    int               fifoOut;
    pthread_t         thread;

    int retval = EXIT_SUCCESS;

    if(signal(SIGINT, handleSignal) == SIG_ERR)
    {
        perror("Error: setting up signal handler.");
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
        close(fifoIn);
        perror("Error: unable to open input fifo in client.");
        retval = EXIT_FAILURE;
        goto done;
    }

    data.fifoIn  = fifoIn;
    data.fifoOut = fifoOut;

    while(terminate == 0)
    {
        char conversion;
        conversion = readChar(data.fifoIn);
        if(conversion != EOF)
        {
            data.conversion = conversion;
            if(pthread_create(&thread, NULL, handleClientRequest, (void *)&data) != 0)
            {
                perror("Error: creating thread");
                retval = EXIT_FAILURE;
                goto cleanup;
            }
            if(pthread_join(thread, NULL) != 0)
            {
                perror("Error: pthread_join in server.");
                retval = EXIT_FAILURE;
                goto cleanup;
            }
        }
    }

    if(terminate == 1)
    {
        display("Signal received! Terminating...");
    }
    display("server ran successfully");
cleanup:
    close(fifoIn);
    close(fifoOut);
done:
    return retval;
}
