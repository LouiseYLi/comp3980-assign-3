#ifndef PROGRAMMING101_NETWORKUTILS_H
#define PROGRAMMING101_NETWORKUTILS_H
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

struct socketNet
{
    int       fd;
    char      conversion;
    char     *ip;
    in_port_t inport;
    in_port_t outport;
};

// in_port_t convert_port(const char *str, int *err);

#endif  // NETWORKUTILS_H