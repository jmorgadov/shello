#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef struct command{
    void* if_command;
    char* name;
    char** args;   
    FILE* in;
    FILE* out;
    int p_out;
    int p_in;
    int return_val;  
    int args_count;
    char* out_symbol;
    char* out_arg;
    char* in_arg;    
}command_t;

typedef struct piped_command{
    command_t* command;
    void* piped_command;
}piped_command_t;

typedef struct logic_command{
    piped_command_t *piped_command;
    char *operator;
    void *logic_command;
}logic_command_t;

typedef struct command_line{
    logic_command_t *logic_command;
    void* command_line;
}command_line_t;

typedef struct if_command{
    command_line_t* if_command_line;
    command_line_t* then_command_line;
    command_line_t* else_command_line;    
}if_command_t;

// if_command_t* init_if_cmd(command_line_t* if_cmd, command_line_t* then_cmd, command_line_t* else_cmd);
// command_line_t* init_cmd_line(logic_command_t** logic_cmds, int cmds_count);
// logic_command_t* init_logic_cmd(piped_command_t* piped_cmd, char* op, void* logic_cmd_right);
// piped_command_t* init_piped_cmd(command_t** cmds, int piped_cmds_count);
// command_t* init_cmd(char* name, char** args, char* out_sym, char* out_arg, char* in_arg);

#endif