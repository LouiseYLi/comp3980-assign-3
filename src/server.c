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

#define CHILD_EXIT 42
static int parseArguments(int argc, char *argv[], void *arg);

#define BACKLOG 5
#define SIZE 128
#define HUNDRED_MILLISECONDS 100000000

typedef char (*convertChar)(char);

void handleSignal(int signal);

// Here I ignored the warning for terminate because I wanted
//  terminate to act as a global flag for handling SIGINT.
//  I couldn't think of an alternative to not using a
//  non-constant global flag that also avoids compiler
//  warnings.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static int terminate = 0;

void handleSignal(int signal)
{
    if(signal == SIGINT)
    {
        terminate = 1;
    }
}

static int parseArguments(int argc, char *argv[], void *arg)
{
    int               option;
    int               retval;
    struct socketNet *data = (struct socketNet *)arg;
    data->ip               = NULL;
    while((option = getopt(argc, argv, "i:")) != -1)
    {
        if(option == 'i')
        {
            data->ip = optarg;
        }
        else
        {
            perror("Error: invalid options.");
            retval = -1;
            goto done;
        }
    }
    if(data->ip == NULL)
    {
        perror("Error: unable to parse ip.");
        retval = -1;
        goto done;
    }

    retval = 0;

done:
    return retval;
}

int main(int argc, char *argv[])
{
    struct socketNet data;
    struct timespec  req;
    struct timespec  rem;

    pid_t pid;
    int   retval;
    int   err;

    const char *PORT = "9999";
    req.tv_sec       = 0;
    req.tv_nsec      = HUNDRED_MILLISECONDS;
    retval           = 0;
    err              = 0;
    data.ip          = NULL;
    data.conversion  = ' ';
    data.server_fd   = 0;
    data.client_fd   = 0;
    data.inport      = convert_port(PORT, &err);
    data.outport     = convert_port(PORT, &err);

    if(signal(SIGINT, handleSignal) == SIG_ERR)
    {
        perror("Error: setting up signal handler.");
        retval = EXIT_FAILURE;
        goto done;
    }

    parseArguments(argc, argv, (void *)&data);

    data.server_fd = open_network_socket_server(data.ip, data.inport, BACKLOG, &err);
    if(data.server_fd < 0)
    {
        perror("Error: opening server-side network socket.");
        retval = -1;
        goto done;
    }

    while(terminate == 0)
    {
        // Sleep just for making sure program isn't using too many resources.
        nanosleep(&req, &rem);
        err            = 0;
        data.client_fd = accept_connection(data.server_fd, &err);
        if(data.client_fd >= 0)
        {
            data.conversion = readChar(data.client_fd);
            pid             = fork();
            displayNum(pid);
            if(pid == -1)
            {
                perror("Error: fork failed");
                retval = -1;
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
                goto waitChild;
            }
        }
        else
        {
            if(err == 0)
            {
                int child_status;
            waitChild:
                waitpid(-1, &child_status, WNOHANG);
                if(WIFEXITED(child_status))
                {
                    display("Child exited normally\n");
                }
                else
                {
                    display("Child did not exit normally\n");
                }
                continue;
            }
            perror("Error: server error accepting connection from client.");
            retval = -1;
            goto cleanup;
        }
    }

    if(terminate == 1)
    {
        display("Signal received! Terminating...");
    }
    display("Server ran successfully");
cleanup:
    if(data.server_fd != 0)
    {
        close(data.server_fd);
    }
    if(data.client_fd != 0)
    {
        close(data.client_fd);
    }
done:
    return retval;
}
