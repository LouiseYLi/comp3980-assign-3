#include "../include/convert.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char upper(char c)
{
    return (char)toupper((unsigned char)c);    // ensures no negative vals avoids errors
}

char lower(char c)
{
    return (char)tolower((unsigned char)c);
}

char none(char c)
{
    return c;
}

void convertBuffer(char *buf, convertChar convert)
{
    int       i     = 0;
    const int count = 128;
    while(i < count && buf[i] != '\0')
    {
        buf[i] = convert(buf[i]);
        ++i;
    }
}

convertChar checkConvertArgs(const char arg)
{
    convertChar convertFunction = NULL;
    if(arg == ' ')
    {
        perror("Error: argument is null.");
        return convertFunction;
    }
    if(arg == 'u')
    {
        convertFunction = upper;
    }
    else if(arg == 'l')
    {
        convertFunction = lower;
    }
    else if(arg == 'n')
    {
        convertFunction = none;
    }
    return convertFunction;
}

char getConvertOption(const char *optionArg)
{
    char result = ' ';
    if(optionArg == NULL)
    {
        perror("Error: argument is null.");
        return result;
    }
    if(strcmp(optionArg, "upper") == 0)
    {
        result = 'u';
    }
    else if(strcmp(optionArg, "lower") == 0)
    {
        result = 'l';
    }
    else if(strcmp(optionArg, "none") == 0)
    {
        result = 'n';
    }
    if(result == ' ')
    {
        perror("Error: invalid arguments.");
    }
    return result;
}
