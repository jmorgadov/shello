#ifndef commandExe_H
#define commandExe_H

#include <stdio.h>
#include "./parser/ast_structs.h"
#define SET_IN(comm, n) comm->in = n
#define SET_OUT(comm, n) comm->out = n

// typedef struct command{
//     char* name;
//     char** args;    
//     FILE* in;
//     FILE* out;
//     int p_out;
//     int p_in;
//     int return_val;    
// }command_t;

// command_t* init_command(char* name, char** args);
// void process_line(char* line);
void execute_shell_line(char* line);
#endif
