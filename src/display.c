#include "../include/display.h"

void display(const char *msg)
{
    printf("%s\n", msg);
}

void displaySize(const ssize_t size)
{
    printf("%d\n", (int)size);
}

void displayNum(int num)
{
    printf("%d\n", (num));
}

void displayChar(char c)
{
    printf("%c", c);
}
