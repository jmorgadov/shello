#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "debug.h"

static int DEBUG;
static char CHAR[2] = {0, 0};

void printArrayInt(int *arr, int count){
    debug("[ ");
    for (int i = 0; i < count; i++) {
        debug("%d ", arr[i]);
    }
    debug("]\n");    
}

char *tostr(char c){
    CHAR[0] = c;
    return CHAR;
}

void set_debug(int d) {
    DEBUG = d;
}

void debug(char *text, ...) {  

    if (!DEBUG) {
        return;
    }

    int i=0;
    va_list ap;
    int d;
    char c, *s;
    float f;
    va_start(ap, text);
    while (text[i]) {
        if (text[i] == '%') {
            i++;
            switch (text[i]) {
            case 's': s = va_arg(ap, char*); printf("%s", s); break;
            case 'd': d = va_arg(ap, int); printf("%d", d); break;
            case 'f': f = va_arg(ap, int); printf("%f", f); break;
            case 'c': c = (char) va_arg(ap, int); printf("%c", c); break;
            }
        }
        else
            printf("%s", tostr(text[i]));
        i++;        
    }
    va_end(ap);
}

void debugc(color_t color, char* text, ...)
{
    if (!DEBUG) {
        return;
    }
    printf("%s", color);
    int i=0;
    va_list ap;
    int d;
    char c, *s;
    float f;
    va_start(ap, text);
    while (text[i]) {
        if (text[i] == '%') {
            i++;
            switch (text[i]) {
            case 's': s = va_arg(ap, char*); printf("%s", s); break;
            case 'd': d = va_arg(ap, int); printf("%d", d); break;
            case 'f': f = va_arg(ap, int); printf("%f", f); break;
            case 'c': c = (char) va_arg(ap, int); printf("%c", c); break;
            }
        }
        else
            printf("%s", tostr(text[i]));
        i++;        
    }
    va_end(ap);
    printf("%s", RESET);
}

void print(char* text, ...){
    printc(RESET, text);
}

void printc(color_t color, char* text, ...)
{
    printf("%s", color);
    int i=0;
    va_list ap;
    int d;
    char c, *s;
    float f;
    va_start(ap, text);
    while (text[i]) {
        if (text[i] == '%') {
            i++;
            switch (text[i]) {
            case 's': s = va_arg(ap, char*); printf("%s", s); break;
            case 'd': d = va_arg(ap, int); printf("%d", d); break;
            case 'f': f = va_arg(ap, int); printf("%f", f); break;
            case 'c': c = (char) va_arg(ap, int); printf("%c", c); break;
            }
        }
        else
            printf("%s", tostr(text[i]));
        i++;        
    }
    va_end(ap);
    write(1,"\033[0m", 5);
}