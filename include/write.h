#ifndef PROGRAMMING101_WRITE_H
#define PROGRAMMING101_WRITE_H
#include "../include/convert.h"
#include "../include/read.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int writeStr(int fifo, const char *buf);

int writeChar(int fifo, char c);

int readUntilNewline(int fd, char *buf);

ssize_t copy(int fd_in, int fd_out, size_t size, int *err);

#endif
