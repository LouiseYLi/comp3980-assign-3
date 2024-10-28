#ifndef PROGRAMMING101_WRITE_H
#define PROGRAMMING101_WRITE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int writeStr(int fifo, const char *buf);

int writeChar(int fifo, char c);

#endif
