#include "ast_structs.h"
#include "../strtools.h"
#include <string.h>
#include <stdlib.h>

char* logic_symbols[2] = { "&&", "||" };
char* io_symbols[3] =  { ">", ">>", "<" };

command_t* parse_command(char *cmd){
    FILE* in = stdin;
    FILE* out = stdout;
    int tokens_count = 0;
    char **comm = split(remove_unnecesary_spaces(cmd), &tokens_count);    
    command_t* temp = init_cmd(comm[0], comm);
    temp->in = in;
    temp->out = out;
    temp->args_count = tokens_count;
    return temp;
}


io_command_t* parse_io_command(char *io_cmd){
    int token_counts = 0;
    int symbol_index = -1;
    int len = strlen(io_cmd);
    int occ = first_occurrense(io_cmd, io_symbols, 2, 0, &symbol_index);
    command_t *cmd = parse_command(io_cmd);

    if (occ != -1){
        io_cmd[occ] == 0;
        return init_io_cmd(cmd, io_symbols[symbol_index], remove_initial_spaces(io_cmd + occ + strlen(io_symbols[symbol_index])));
    }
    else{
        return init_io_cmd(cmd, NULL, NULL);
    }
}


piped_command_t* parse_piped_command(char *piped_cmd){
    char* spl[1];
    spl[0] = " | ";
    int tokens_count = 0;
    char **cmds = splitby(piped_cmd, &tokens_count, spl, 1);
    io_command_t **io_commands = (io_command_t**)malloc(sizeof(logic_command_t*)*tokens_count);

    for (int i = 0; i < tokens_count; i++){        
        io_commands[i] = parse_io_command(cmds[i]);
    }    

    return init_piped_cmd(io_commands, tokens_count - 1);
}

logic_command_t* parse_logic_command(char *logic_cmd){
    int token_counts = 0;
    int symbol_index = -1;
    logic_command_t* right = NULL;
    int len = strlen(logic_cmd);
    int occ = first_occurrense(logic_cmd, logic_symbols, 2, 0, &symbol_index);
    if (occ != -1){
        logic_cmd[occ] == 0;
        char *temp = (char*)malloc(sizeof(char)*(occ + 1));
        strcpy(temp, logic_cmd);
        temp[occ] = 0;
        piped_command_t *io_cmd = parse_piped_command(temp);
        right = (void*)parse_logic_command(logic_cmd + occ + 2);
        return init_logic_cmd(io_cmd, logic_symbols[symbol_index], (void*)right);
    }
    else{
        piped_command_t *io_cmd = parse_piped_command(logic_cmd);
        return init_logic_cmd(io_cmd, NULL, NULL);
    }
}


command_line_t* parse_command_line(char *line){
    char* spl[1];
    spl[0] = ";";
    int tokens_count = 0;;
    char **cmds = splitby(line, &tokens_count, spl, 1);
    logic_command_t **logic_cmds = (logic_command_t**)malloc(sizeof(logic_command_t*)*tokens_count);

    for (int i = 0; i < tokens_count; i++){        
        logic_cmds[i] = parse_logic_command(cmds[i]);
    }    
    return init_cmd_line(logic_cmds, tokens_count);
}

void cmd_str(command_t* cmd, char* dest, int* index){
    int idx = *index;
    int cmd_len = strlen(cmd->name);
    for (int i = 0; i < cmd_len; i++){
        dest[idx++] = cmd->name[i];
    }
    for (int i = 1; i < cmd->args_count; i++)
    {
        dest[idx++] = ' ';
        int arg_len = strlen(cmd->args[i]);
        for (int j = 0; j < arg_len; j++){
            dest[idx++] = cmd->args[i][j];
        }
    }    
    *index = idx;
}

void io_str(io_command_t* cmd, char* dest, int* index){
    cmd_str(cmd->command, dest, index);

    if (!cmd->symbol) return;

    int sym_len = strlen(cmd->symbol);
    int idx = *index;
    for (int i = 0; i < sym_len; i++){
        dest[idx++] = cmd->symbol[i];
    }
    dest[idx++] = ' ';
    *index += 1;
    int path_len = strlen(cmd->file_path);
    for (int i = 0; i < path_len; i++){
        dest[idx++] = cmd->file_path[i];
    }    
    *index = idx;
}

void pipe_str(piped_command_t* cmd, char* dest, int* index){
    io_str(cmd->io_command[0], dest, index);
    int idx = *index;
    for (int i = 0; i < cmd->pipes_count; i++)
    {
        dest[idx++] = ' ';
        dest[idx++] = '|';
        dest[idx++] = ' ';
        io_str(cmd->io_command[i + 1], dest, index);
    }
    *index = idx;
    
}

void logic_str(logic_command_t* cmd, char* dest, int* index){
    if (cmd->operator){
        pipe_str(cmd->piped_command, dest, index);
        int idx = *index;
        dest[idx++] = ' '; 
        dest[idx++] = cmd->operator[0]; 
        dest[idx++] = cmd->operator[1]; 
        dest[idx++] = ' '; 
        *index = idx;
        logic_str((logic_command_t*)cmd->logic_command, dest, index);                
    }    
    else{
        pipe_str(cmd->piped_command, dest, index);
    }
}

char* line_str(command_line_t* cmd){
    char* str = (char*)malloc(sizeof(char)*500);
    int index = 0;
    logic_str(cmd->logic_commands[0], str, &index);
    for (int i = 1; i < cmd->commands_count; i++)
    {
        str[index++] = ';';
        str[index++] = ' ';
        logic_str(cmd->logic_commands[i], str, &index);
    }    
    char* final_str = (char*)malloc(sizeof(char)*index);
    strncpy(final_str, str, index);
    return final_str;
}