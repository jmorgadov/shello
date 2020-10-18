#include "ast_structs.h"
#include "tokenizer.h"
#include "../strtools.h"
#include "../debug.h"
#include <string.h>
#include <stdlib.h>

char* logic_symbols[2] = { "&&", "||" };
char* io_symbols[3] =  { ">", ">>", "<" };
char* cond[4] =  { "if", "then", "else", "end" };

#define INIT(type) (type*)malloc(sizeof(type))

#define THEN_ET 0b001
#define ELSE_ET 0b010
#define END_ET  0b100

void* print_error(char* message, int* error){
    printc(BOLD_RED, "%s\n", message);
    *error = 1;
    return NULL;
}

int type_match_end_type(int type, int end_type){
    int b_type = 0;
    if (type == COND_THEN)
        b_type = 1;
    else if (type == COND_ELSE)
        b_type = 2;
    else if (type == COND_END)
        b_type = 4;
    return b_type & end_type;
}

command_line_t* try_parse_cmd_line(token_t** tokens, int len, int* index, int* error, int end_type);

if_command_t* try_parse_if_cmd(token_t** tokens, int len, int* index, int* error, int end_type){
    if_command_t* if_cmd = INIT(if_command_t);
    if_cmd->if_command_line = try_parse_cmd_line(tokens, len, index, error, THEN_ET | ELSE_ET);
    if (*error){
        // ERROR PARSING IF COND
        // return print_error("Message", error);
    }
    else{
        if (*index >= len){
            // ERROR NOT ELSE OR END COMMAND FOUND
            return print_error("Token \'then\' or \'else\' not found after \'if\'", error);
        }
        int last_cond_type = tokens[*index]->type;
        *index = *index + 1;
        if (last_cond_type == COND_THEN){
            if_cmd->then_command_line = try_parse_cmd_line(tokens, len, index, error, ELSE_ET | END_ET);
            if (*error){
                // ERROR PARSING THEN COND
                // return print_error("Message", error);
            }
            else{
                if (*index >= len){
                    // ERROR NOT ELSE OR END COMMAND FOUND
                    return print_error("Token \'else\' or \'end\' not found after \'then\'", error);
                }
                last_cond_type = tokens[*index]->type;
                *index = *index + 1;
            }            
        }

        if (last_cond_type == COND_ELSE){
            if_cmd->else_command_line = try_parse_cmd_line(tokens, len, index, error, END_ET);
            if (*error){
                // ERROR PARSING ELSE COND
                // return print_error("Message", error);
            }
            else{
                if (*index >= len){
                    // ERROR NOT ELSE OR END COMMAND FOUND
                    return print_error("Token \'end\' not found after \'else\'", error);
                }
                last_cond_type = tokens[*index]->type;
                *index = *index + 1;
            }
        }

        if (last_cond_type != COND_END){
            // ERROR
            return print_error("Token \'end\' not found", error);
        }

    }
    return if_cmd;
}

command_t* try_parse_cmd(token_t** tokens, int len, int* index, int* error, int end_type){

    command_t* cmd = INIT(command_t);
    cmd->if_command = NULL;
    cmd->return_val = 0;
    cmd->out_symbol = NULL;
    cmd->out_arg = NULL;
    cmd->in_arg = NULL;
    int command_parsed = 0;

    char** cmd_args = (char**)calloc(len + 1, sizeof(char*));
    int args_index = 0;

    for (; *index < len; *index = *index + 1)
    {        
        token_t* current = tokens[*index];
        char* token_repr = get_token_repr(current);
        if (current->type == IO_IN){
            if (*index + 1 >= len){
                // ERROR NO PATH FOUND
                return print_error("Path not found for command <", error);
            }
            *index = *index + 1;
            char* next_token_rep = get_token_repr(tokens[*index]);
            if (next_token_rep){
                cmd->in_arg = next_token_rep;
            }
            else{
                // ERROR INVALID PATH
                return print_error("Invalid path for command <", error);
            }
        }
        else if (current->type == IO_OUT){
            if (*index + 1 >= len){
                // ERROR NO PATH
                return print_error("Path not found for command >", error);
            }
            *index = *index + 1;
            char* next_token_rep = get_token_repr(tokens[*index]);
            if (next_token_rep){
                cmd->out_symbol = ">";
                cmd->out_arg = next_token_rep;
            }
            else{
                // ERROR INVALID PATH
                return print_error("Invalid path for command >", error);
            }

        }
        else if (current->type == IO_OUT_A){
            if (*index + 1 >= len){
                // ERROR NO PATH
                return print_error("Path not found for command >>", error);
            }
            *index = *index + 1;
            char* next_token_rep = get_token_repr(tokens[*index]);
            if (next_token_rep){
                cmd->out_symbol = ">>";
                cmd->out_arg = next_token_rep;
            }
            else{
                // ERROR INVALID PATH
                return print_error("Invalid path for command >>", error);
            }

        }
        else if (is_symbol_token(current)){
            if (!command_parsed){
                // ERROR NO COMMAND FOUND
                return print_error("Command not found", error);
            }
            break;
        }
        else if (type_match_end_type(current->type, end_type)){
            if (!command_parsed){
                // ERROR NO COMMAND FOUND
                return print_error("Command not found", error);
            }
            // *index = *index + 1;
            break;
        }
        else if (current->type == COND_IF && !command_parsed){
            *index = *index + 1;
            cmd->if_command = (void*)try_parse_if_cmd(tokens, len, index, error, end_type);
            if (*error){
                // ERROR PARSING IF CMD
                return NULL;
            }
            command_parsed = 2;
            *index = *index - 1;
        }
        else if (!command_parsed){
            command_parsed = 1;
            cmd_args[args_index++] = token_repr;
        }
        else if (command_parsed == 1){
            cmd_args[args_index++] = token_repr;
        }
        else if (command_parsed == 2){
            // ERROR UNESPECTED TOKEN
            printc(BOLD_RED, "Unspected token \'%s\'\n", token_repr);
            *error = 1;
            return NULL;
        }
    }

    if (!command_parsed){
        // ERROR NO COMMAND FOUND
        return print_error("No command found", error);
    }

    cmd_args[args_index] = NULL;
    cmd->args_count = args_index;
    cmd->args = cmd_args;
    cmd->name = cmd_args[0];
    cmd->p_in = 0;
    cmd->p_out = 1;
    return cmd;    
}

piped_command_t* try_parse_piped_cmd(token_t** tokens, int len, int* index, int* error, int end_type){
    piped_command_t* ppc = INIT(piped_command_t);
    ppc->command = try_parse_cmd(tokens, len, index, error, end_type);
    if (*error){
        // ERROR PARSING LOGIC COMMAND
        // return print_error("Message", error);
    }
    else if (*index < len - 1 && tokens[*index]->type == PIPE){
        *index = *index + 1;
        ppc->piped_command = (void*)try_parse_piped_cmd(tokens, len, index, error, end_type);
        if (*error){
            // ERROR PARSING CMD
            // return print_error("Message", error);
        }
    }
    else{
        ppc->piped_command = NULL;
    }
    return ppc;
}

logic_command_t* try_parse_logic_cmd(token_t** tokens, int len, int* index, int* error, int end_type){
    logic_command_t* lc = INIT(logic_command_t);
    lc->piped_command = try_parse_piped_cmd(tokens, len, index, error, end_type);
    if (*error){
        // ERROR PARSING PIPED CMD
        // return print_error("Message", error);
    }
    else if (*index < len - 1 && tokens[*index]->type == LOGIC_AND){
        *index = *index + 1;
        lc->operator = logic_symbols[0];
        lc->logic_command = try_parse_logic_cmd(tokens, len, index, error, end_type);
        if (*error){
            // ERROR PARSING LOGIC_CMD
            // return print_error("Message", error);
        }
    }
    else if (*index < len - 1 && tokens[*index]->type == LOGIC_OR){
        *index = *index + 1;
        lc->operator = logic_symbols[1];
        lc->logic_command = try_parse_logic_cmd(tokens, len, index, error, end_type);
        if (*error){
            // ERROR PARSING LOGIC_CMD
            // return print_error("Message", error);
        }
    }
    else{
        lc->operator = NULL;
        lc->logic_command = NULL;
    }
    return lc;
}

command_line_t* try_parse_cmd_line(token_t** tokens, int len, int* index, int* error, int end_type){
    command_line_t* cmd = INIT(command_line_t);
    cmd->logic_command = try_parse_logic_cmd(tokens, len, index, error, end_type);
    if (*error){
        // ERROR PARSING LOGIC COMMAND
        // return print_error("Message", error);
    }
    else if (*index < len - 1 && tokens[*index]->type == SEMICOLON){
        *index = *index + 1;
        cmd->command_line = (void*)try_parse_cmd_line(tokens, len, index, error, end_type);
        if (*error){
            // ERROR PARSING CMD_LINE
            // return print_error("Message", error);
        }
    }    
    else{
        cmd->command_line = NULL;
    }
    return cmd;
}


command_line_t* parse_line(char* line, char** line_repr, int* error){
    int token_count = 0;    
    token_t** tokens = get_tokens(line, &token_count, error);    
    int index = 0;
    *line_repr = get_tokens_repr(tokens, token_count);    
    return try_parse_cmd_line(tokens, token_count, &index, error, 0);
}