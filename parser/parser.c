#include "ast_structs.h"
#include "../strtools.h"
#include <string.h>
#include <stdlib.h>

char* logic_symbols[2] = { "&&", "||" };
char* io_symbols[3] =  { ">", ">>", "<" };
char* cond[4] =  { "if", "then", "else", "end" };

command_t* parse_command(char *cmd){
    FILE* in = stdin;
    FILE* out = stdout;
    int tokens_count = 0;
    char **comm = split(remove_unnecesary_spaces(cmd), &tokens_count);
    for (int i = 0; i < tokens_count; i++)
    {
        comm[i] = remove_str_repr(comm[i]);
    }
      
    command_t* temp = init_cmd(comm[0], comm);
    temp->in = in;
    temp->out = out;
    temp->args_count = tokens_count;
    return temp;
}


io_command_t* parse_io_command(char *io_cmd){
    int symbol_index = -1;
    int occ = first_occurrense(io_cmd, io_symbols, 2, 0, &symbol_index);
    command_t *cmd = parse_command(io_cmd);

    if (occ != -1){
        io_cmd[occ] = 0;
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
    int symbol_index = -1;
    logic_command_t* right = NULL;
    int occ = first_occurrense(logic_cmd, logic_symbols, 2, 0, &symbol_index);
    if (occ != -1){
        logic_cmd[occ] = 0;
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


command_line_t* parse_command_line(char *cmd_line){
    char* spl[1];
    spl[0] = ";";
    int tokens_count = 0;;
    char **cmds = splitby(cmd_line, &tokens_count, spl, 1);
    logic_command_t **logic_cmds = (logic_command_t**)malloc(sizeof(logic_command_t*)*tokens_count);

    for (int i = 0; i < tokens_count; i++){        
        logic_cmds[i] = parse_logic_command(cmds[i]);
    }    
    return init_cmd_line(logic_cmds, tokens_count);
}

int match_next_cond(char* cond1, char* cond2){
    if (strcmp(cond1, "if") == 0){
        return strcmp(cond2, "then") == 0 || strcmp(cond2, "else") == 0;
    }
    else if (strcmp(cond1, "then") == 0){
        return strcmp(cond2, "else") == 0 || strcmp(cond2, "end") == 0;
    }
    else if (strcmp(cond1, "else") == 0){
        return strcmp(cond2, "end") == 0;
    }
    return 0;
}

line_t* parse_line(char* line, int* line_index, int last_cond){    

    int len = strlen(line);

    line_t* if_cmd = NULL;
    line_t* then_cmd = NULL;
    line_t* else_cmd = NULL;

    int index = *line_index;
    while (index < len && line[index] == ' ') index++;
    *line_index = index;
    
    int cond_index = -1;
    int occ = first_occurrense(line + *line_index, cond, 4, 0, &cond_index);

    if ((last_cond == -1 && cond_index != 0 ) || (last_cond != -1 && cond_index != 0 && match_next_cond(cond[last_cond], cond[cond_index]) && occ != -1)){
        int sub_line_len = occ != -1 ? occ : len - *line_index;
        char* next_comm = (char*)calloc(sub_line_len, sizeof(char));
        strncpy(next_comm, line + *line_index, sub_line_len);
        command_line_t* cmd_line = parse_command_line(next_comm);
        *line_index += occ; 
        return init_line(cmd_line, if_cmd, then_cmd, else_cmd);        
    }

    // IF ...
    if (cond_index == 0){
        *line_index = index + 2;
        if_cmd = parse_line(line, line_index, 0);
        
        // int index = *line_index;
        // while (index < len && line[index] == ' ') index++;
        cond_index = -1;
        occ = first_occurrense(line + *line_index, cond, 4, 0, &cond_index);

        // IF ... THEN ...
        if (cond_index == 1){
            *line_index += occ + 4;
            then_cmd = parse_line(line, line_index, 1);

            cond_index = -1;
            occ = first_occurrense(line + *line_index, cond, 4, 0, &cond_index);

            // IF ... THEN ... END
            if (cond_index == 3){
                *line_index += occ + 3;                
            }

            // IF ... THEN ... ELSE ...
            else if (cond_index == 2){
                *line_index += occ + 4;
                else_cmd = parse_line(line, line_index, 2);

                cond_index = -1;
                occ = first_occurrense(line + *line_index, cond, 4, 0, &cond_index);

                // IF ... THEN ... ELSE ... END
                if (cond_index != 3){
                    printf("ERORR!!!!!!! NO END ---\n");
                }
                else{
                    *line_index += occ + 3;
                }
            }
        }
        // IF ... ELSE ...
        else if (cond_index == 2){
            *line_index += occ + 4;
            else_cmd = parse_line(line, line_index, 2);

            cond_index = -1;
            occ = first_occurrense(line + *line_index, cond, 4, 0, &cond_index);

            // IF ... ELSE ... END
            if (cond_index != 3){
                printf("ERORR!!!!!!! NO END ---\n");
                // error
            }
            else{
                *line_index += occ + 3;
            }
        }
    }
    return init_line(NULL, if_cmd, then_cmd, else_cmd);
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

void cmd_line_str(command_line_t* cmd, char* dest, int* index){
    logic_str(cmd->logic_commands[0], dest, index);    
    int idx = *index;
    dest[idx++] = ' ';
    *index = idx;
    for (int i = 1; i < cmd->commands_count; i++)
    {
        int idx = *index;
        dest[idx++] = ';';
        dest[idx++] = ' ';
        *index = idx;
        logic_str(cmd->logic_commands[i], dest, index);
    }    
}

void line_str_rec(line_t* line, char* dest, int* index){
    int idx = 0;
    if (line->if_command_line){
        idx = *index;
        dest[idx++] = 'i';
        dest[idx++] = 'f';
        dest[idx++] = ' ';
        *index = idx;
        line_str_rec(line->if_command_line, dest, index);
        if (line->then_command_line){
            idx = *index;
            dest[idx++] = 't';
            dest[idx++] = 'h';
            dest[idx++] = 'e';
            dest[idx++] = 'n';
            dest[idx++] = ' ';
            *index = idx;
            line_str_rec(line->then_command_line, dest, index);
        }
        if (line->else_command_line){
            idx = *index;
            dest[idx++] = 'e';
            dest[idx++] = 'l';
            dest[idx++] = 's';
            dest[idx++] = 'e';
            dest[idx++] = ' ';
            *index = idx;
            line_str_rec(line->else_command_line, dest, index);
        }
        idx = *index;
        dest[idx++] = 'e';
        dest[idx++] = 'n';
        dest[idx++] = 'd';
        dest[idx++] = ' ';
        *index = idx;
    }
    else{
        cmd_line_str(line->command_line, dest, index);
    }
}

char* line_str(line_t* cmd){
    char* str = (char*)malloc(sizeof(char)*500);
    int index = 0;

    line_str_rec(cmd, str, &index);
    char* final_str = (char*)calloc(index, sizeof(char));
    strncpy(final_str, str, index);
    return final_str;
}