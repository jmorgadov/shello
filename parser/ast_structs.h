#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef struct command{
    char* name;
    char** args;    
    char** raw_args;    
    FILE* in;
    FILE* out;
    int p_out;
    int p_in;
    int return_val;  
    int args_count;  
    int raw_args_count;  
    char* out_symbol;
    char* out_arg;
    char* in_arg;    
}command_t;

typedef struct piped_command{
    command_t **commands;
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

typedef struct line{
    command_line_t* command_line;
    void* if_command_line;
    void* then_command_line;
    void* else_command_line;
}line_t;

command_line_t* init_cmd_line(logic_command_t** logic_cmds, int cmds_count);
logic_command_t* init_logic_cmd(piped_command_t* piped_cmd, char* op, void* logic_cmd_right);
piped_command_t* init_piped_cmd(command_t** cmds, int piped_cmds_count);
command_t* init_cmd(char* name, char** args, char* out_sym, char* out_arg, char* in_arg);
line_t* init_line(command_line_t* cmd, line_t* if_cmd, line_t* then_cmd, line_t* else_cmd);

#endif