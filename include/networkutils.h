#ifndef PROGRAMMING101_NETWORKUTILS_H
#define PROGRAMMING101_NETWORKUTILS_H
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Compiler thinks that I'm not using the struct members because the struct
//  is in a separate file.
// I made struct in a separate file to import because I didn't want to
//  declare it multiple times for the program, that's why I disabled the
//  unusedStructMember warnings.
struct socketNet
{
    // cppcheck-suppress unusedStructMember
    int server_fd;
    // cppcheck-suppress unusedStructMember
    int client_fd;
    // cppcheck-suppress unusedStructMember
    char conversion;
    // cppcheck-suppress unusedStructMember
    char *ip;
    // cppcheck-suppress unusedStructMember
    in_port_t inport;
    // cppcheck-suppress unusedStructMember
    in_port_t outport;
};

in_port_t convert_port(const char *str, int *err);

#endif    // NETWORKUTILS_H
