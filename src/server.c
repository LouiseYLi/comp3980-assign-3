#include "../include/convert.h"
#include "../include/display.h"
#include "../include/networkutils.h"
#include "../include/open.h"
#include "../include/read.h"
#include "../include/write.h"
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

// #define FIFO_INPUT "./fifo/input"
// #define FIFO_OUTPUT "./fifo/output"
#define CHILD_EXIT 42
#define ERR_NONE 0
#define ERR_NO_DIGITS 1
#define ERR_OUT_OF_RANGE 2
#define ERR_INVALID_CHARS 3
// static void     *handleClientRequest(void *arg);
static in_port_t convert_port(const char *str, int *err);
int              parseArguments(int argc, char *argv[], void *arg);

#define BACKLOG 5
#define SIZE 128

typedef char (*convertChar)(char);

void handleSignal(int signal);

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;

// struct socketNet
// {
//     int       fd;
//     char      conversion;
//     char     *ip;
//     in_port_t inport;
//     in_port_t outport;
// };

void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        terminate = 1;
    }
}

int parseArguments(int argc, char *argv[], void *arg)
{
    int               option;
    int               retval = 0;
    struct socketNet *data   = (struct socketNet *)arg;
    data->ip                 = NULL;
    while((option = getopt(argc, argv, "i:")) != -1)
    {
        if(option == 'i')
        {
            data->ip = optarg;
        }
        else
        {
            perror("Error: invalid options.");
            retval = EXIT_FAILURE;
            goto done;
        }
    }
    if(data->ip == NULL)
    {
        perror("Error: unable to parse ip.");
        retval = EXIT_FAILURE;
        goto done;
    }

    retval = 0;

done:
    return retval;
}

// static void *handleClientRequest(void *arg)
// {
//     const struct socketNet *data = (struct socketNet *)arg;
//     convertChar              convertFunction;
//     char                     currentChar;
//     convertFunction = checkConvertArgs(data->conversion);
//     if(convertFunction == NULL)
//     {
//         perror("Error: obtaining specific convert function");
//         return NULL;
//     }
//     while((currentChar = readChar(data->fd)) != EOF)
//     {
//         if(writeChar(data->fd, convertFunction(currentChar)) == -1)
//         {
//             perror("Error: error writing to fifo.");
//             return NULL;
//         }
//         if(currentChar == '\0')
//         {
//             break;
//         }
//     }
//     return NULL;
// }

// int parseArguments(int argc, char *argv[], void *arg)
// {
//     int               option;
//     int               retval = 0;
//     struct socketNet *data   = (struct socketNet *)arg;
//     data->ip                 = NULL;
//     while((option = getopt(argc, argv, "i:")) != -1)
//     {
//         if(option == 'i')
//         {
//             data->ip = optarg;
//         }
//         else
//         {
//             perror("Error: invalid options.");
//             retval = EXIT_FAILURE;
//             goto done;
//         }
//     }
//     if(data->ip == NULL)
//     {
//         perror("Error: unable to parse ip.");
//         retval = EXIT_FAILURE;
//         goto done;
//     }

//     retval = 0;

// done:
//     return retval;
// }

int main(int argc, char *argv[])
{
    struct socketNet data;
    // pthread_t         thread;
    pid_t pid;

    int         retval = EXIT_SUCCESS;
    int         err;
    const char *PORT = "9999";

    if(signal(SIGINT, handleSignal) == SIG_ERR)
    {
        perror("Error: setting up signal handler.");
        retval = EXIT_FAILURE;
        goto done;
    }

    err          = 0;
    data.fd      = 0;
    data.inport  = convert_port(PORT, &err);
    data.outport = convert_port(PORT, &err);

    parseArguments(argc, argv, (void *)&data);

    data.fd = open_network_socket_server(data.ip, data.inport, BACKLOG, &err);

    // fd = open_network_socket_server()
    // if(fd == -1)
    // {
    //     perror("Error: unable to open input fifo in client.");
    //     retval = EXIT_FAILURE;
    //     goto done;
    // }
    // fifoOut = open(FIFO_OUTPUT, O_RDWR | O_CLOEXEC);
    // if(fifoOut == -1)
    // {
    // close(fd);
    // perror("Error: unable to open input fifo in client.");
    // retval = EXIT_FAILURE;
    // goto done;
    // }

    // data.fifoOut = fifoOut;
    display(data.ip);
    while(terminate == 0)
    {
        err             = 0;
        data.conversion = readChar(data.fd);
        if(data.conversion != EOF)
        {
            pid = fork();
            if(pid == -1)
            {
                perror("Error: fork failed");
                retval = EXIT_FAILURE;
                goto cleanup;
            }
            if(pid == 0)
            {
                display("Child process\n");
                copy(SIZE, &err, (void *)&data);
                _exit(CHILD_EXIT);
            }
            else
            {
                int child_status;
                waitpid(pid, &child_status, 0);
                if(WIFEXITED(child_status))
                {
                    display("Child exited normally\n");
                    // display("Child exited with status %d\n", WEXITSTATUS(child_status));
                }
                else
                {
                    display("Child did not exit normally\n");
                }
            }
            // if(pthread_create(&thread, NULL, handleClientRequest, (void *)&data) != 0)
            // {
            //     perror("Error: creating thread");
            //     retval = EXIT_FAILURE;
            //     goto cleanup;
            // }
            // if(pthread_join(thread, NULL) != 0)
            // {
            //     perror("Error: pthread_join in server.");
            //     retval = EXIT_FAILURE;
            //     goto cleanup;
            // }
        }
    }

    if(terminate == 1)
    {
        display("Signal received! Terminating...");
    }
    display("server ran successfully");
cleanup:
    if(data.fd != 0)
    {
        close(data.fd);
    }
    // close(fifoOut);
    free(data.ip);
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
