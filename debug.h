#ifndef DEBUG_H
#define DEBUG_H

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define BOLD_GREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define BOLD_YELLOW "\033[1;33m"
#define BLUE "\033[0;34m"
#define BOLD_BLUE "\033[10;34m"
#define MAGENTA "\033[0;35m"
#define BOLD_MAGENTA "\033[1;35m"
#define CYAN "\033[0;36m"
#define BOLD_CYAN "\033[1;36m"
#define RESET "\033[0m"

typedef char* color_t;


void printArrayInt(int *arr, int count);

/* Set the debug variable to show the logs or not */
void set_debug(int d);

/* Log to the console a text */
void debug(char *text, ...);

void debugc(color_t color, char* text, ...);

void print(char *text, ...);

void printc(color_t color, char* text, ...);

#endif