#ifndef PROGRAMMING101_CONVERT_H
#define PROGRAMMING101_CONVERT_H

typedef char (*convertChar)(char);

char upper(char c);

char lower(char c);

char none(char c);

convertChar checkConvertArgs(char arg);

char getConvertOption(const char *optionArg);

#endif
