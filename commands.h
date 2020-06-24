#ifndef commandExe_H
#define commandExe_H

#include <stdio.h>
#define SET_IN(comm, n) comm->in = n
#define SET_OUT(comm, n) comm->out = n

typedef struct command{
    char* name;
    char** args;    
    FILE* in;
    FILE* out;
    int p_out;
    int p_in;
}command_t;

void execute(command_t* command);
command_t* init_command(char* name, char** args);
void execute_line(char** command_tokens, int tokens_count);
#endif
