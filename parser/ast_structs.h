#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef struct command{
    char* name;
    char** args;    
    FILE* in;
    FILE* out;
    int p_out;
    int p_in;
    int return_val;  
    int args_count;  
}command_t;

typedef struct io_command{
    command_t *command;
    char *symbol;
    char *file_path;
}io_command_t;

typedef struct piped_command{
    io_command_t **io_command;
    int pipes_count;
}piped_command_t;

typedef struct logic_command{
    piped_command_t *piped_command;
    char *operator;
    void *logic_command;
}logic_command_t;

typedef struct command_line{
    logic_command_t **logic_commands;
    int commands_count;
}command_line_t;

command_line_t* init_cmd_line(logic_command_t** logic_cmds, int cmds_count);
logic_command_t* init_logic_cmd(piped_command_t* piped_cmd, char* op, void* logic_cmd_right);
piped_command_t* init_piped_cmd(io_command_t** io_cmds, int piped_cmds_count);
io_command_t* init_io_cmd(command_t* cmd, char* symbol, char* file_path);
command_t* init_cmd(char* name, char** args);

#endif