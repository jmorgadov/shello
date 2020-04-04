#ifndef HISTORY_HANDLER
#define HISTORY_HANDLER
#include "commands.h"


#define HISTORY_MAX_SIZE 10

typedef struct history_handler
{
    int count;
    int index;
    char** lines;
}history_h;

history_h* init_history_handler();
int add_line (command_t* comand,history_h* hh);
char** get_history_lines (history_h* hh);
#endif