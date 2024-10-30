#ifndef PROGRAMMING101_READ_H
#define PROGRAMMING101_READ_H
#include "../include/display.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

char readChar(int fd);

// int parseArguments(int argc, char *argv[], void *arg);

#endif
