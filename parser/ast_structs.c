#include "ast_structs.h"
#include <stdlib.h>

#define RESERVE(type) (type*)malloc(sizeof(type))

line_t* init_line(command_line_t* cmd, line_t* if_cmd, line_t* then_cmd, line_t* else_cmd){
    line_t* line = (line_t*)malloc(sizeof(line_t));
    line->command_line = cmd;
    line->if_command_line = (void*)if_cmd;
    line->then_command_line = (void*)then_cmd;
    line->else_command_line = (void*)else_cmd;
    return line;
}

command_line_t* init_cmd_line(logic_command_t** logic_cmds, int cmds_count){
    command_line_t* cmd_line = RESERVE(command_line_t);
    cmd_line->logic_commands = logic_cmds;
    cmd_line->commands_count = cmds_count;
    return cmd_line;
}

logic_command_t* init_logic_cmd(piped_command_t* piped_cmd, char* op, void* logic_cmd_right){
    logic_command_t* logic_cmd = RESERVE(logic_command_t);
    logic_cmd->piped_command = piped_cmd;
    logic_cmd->operator = op;
    logic_cmd->logic_command = logic_cmd_right;
    return logic_cmd;
}

piped_command_t* init_piped_cmd(io_command_t** io_cmds, int piped_cmds_count){
    piped_command_t* piped_cmd = RESERVE(piped_command_t);
    piped_cmd->io_command = io_cmds;
    piped_cmd->pipes_count = piped_cmds_count;
    return piped_cmd;
}

io_command_t* init_io_cmd(command_t* cmd, char* symbol, char* file_path){
    io_command_t* io_cmd = RESERVE(io_command_t);
    io_cmd->command = cmd;
    io_cmd->symbol = symbol;
    io_cmd->file_path = file_path;
    return io_cmd;
}

command_t* init_cmd(char* name, char** args){
    command_t* comm = (command_t*)malloc(sizeof(command_t));
    comm->name = name;
    comm->args = args;
    comm->in = NULL;
    comm->out = NULL;  
    comm->p_in = 0;
    comm->p_out = 1;  
    return comm;
}
