#include "ast_structs.h"
#include <stdlib.h>

#define RESERVE(type) (type*)malloc(sizeof(type))

// if_command_t* init_if_cmd(command_line_t* if_cmd, command_line_t* then_cmd, command_line_t* else_cmd){
//     if_command_t* ifcmd = (if_command_t*)malloc(sizeof(if_command_t));
//     ifcmd->if_command_line = if_cmd;
//     ifcmd->then_command_line = then_cmd;
//     ifcmd->else_command_line = else_cmd;
//     return ifcmd;
// }

// command_line_t* init_cmd_line(logic_command_t** logic_cmds, int cmds_count){
//     command_line_t* cmd_line = RESERVE(command_line_t);
//     cmd_line->logic_commands = logic_cmds;
//     cmd_line->commands_count = cmds_count;
//     return cmd_line;
// }

// logic_command_t* init_logic_cmd(piped_command_t* piped_cmd, char* op, void* logic_cmd_right){
//     logic_command_t* logic_cmd = RESERVE(logic_command_t);
//     logic_cmd->piped_command = piped_cmd;
//     logic_cmd->operator = op;
//     logic_cmd->logic_command = logic_cmd_right;
//     return logic_cmd;
// }

// piped_command_t* init_piped_cmd(command_t** cmds, int piped_cmds_count){
//     piped_command_t* piped_cmd = RESERVE(piped_command_t);
//     piped_cmd->commands = cmds;
//     piped_cmd->pipes_count = piped_cmds_count;
//     return piped_cmd;
// }

// io_command_t* init_io_cmd(command_t* cmd, char* symbol, char* file_path){
//     io_command_t* io_cmd = RESERVE(io_command_t);
//     io_cmd->command = cmd;
//     io_cmd->symbol = symbol;
//     io_cmd->file_path = file_path;
//     return io_cmd;
// }

// command_t* init_cmd(char* name, char** args, char* out_sym, char* out_arg, char* in_arg){
//     command_t* comm = (command_t*)malloc(sizeof(command_t));
//     comm->name = name;
//     comm->args = args;
//     comm->in = NULL;
//     comm->out = NULL;  
//     comm->p_in = 0;
//     comm->p_out = 1;  
//     comm->out_symbol = out_sym;
//     comm->out_arg = out_arg;
//     comm->in_arg = in_arg;
//     return comm;
// }
