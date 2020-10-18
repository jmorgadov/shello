#include "ast_structs.h"
#include "../strtools.h"
#include "../debug.h"
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>

char* quotes[1] = {"\""};

token_t* init_token(int token_type, char* value){
    token_t* tok = (token_t*)malloc(sizeof(token_t));
    tok->type = token_type;
    tok->value = value;
    return tok;
}

char* current_token_value = NULL;
int current_value_index = 0;

token_t* get_token(char* t_value, int tv_len, int t_type){
    t_value[tv_len] = 0;
    char* value = (char*)calloc(tv_len + 1, sizeof(char));
    strncpy(value, t_value, tv_len);
    // free(current_token_value);
    current_value_index = 0;
    return init_token(t_type, value);
}

int is_letter(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


token_t** get_tokens(char* line, int* tokens_count, int* error){

    int len = strlen(line);

    current_token_value = (char*)calloc(len + 1, sizeof(char));
    current_value_index = 0;

    token_t** tokens = (token_t**)calloc(len, sizeof(token_t*));
    int token_index = 0;

    int on_string = 0;
    int tt = 0;
    int add_token = 0;

    for (int i = 0; i < len; i++)
    {
        char current_char = line[i];
        if (on_string){
            if (current_char == '\"'){
                on_string = 0;
            }
            else if (current_char == '\\'){
                if (i < len - 1){
                    i++;
                    current_token_value[current_value_index++] = line[i];
                }
                else{
                    //ERROR
                }
            }
            else{
                current_token_value[current_value_index++] = current_char;                                
            }
        }
        else{
            if (current_char == '\"'){
                on_string = 1;
            }
            else if (current_char == ' '){
                if (current_value_index)
                    add_token = 1;
                while (i < len - 1 && line[i + 1] == ' '){
                    i++;
                }
            }
            else if (current_char == '\\'){
                if (i < len - 1){
                    i++;
                    current_token_value[current_value_index++] = line[i];
                }
                else{
                    //ERROR
                }
            }
            else if (current_char == ';'){
                if (current_value_index){
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                tt = SEMICOLON;
            }
            else if (starts_with(line + i, ">>")){
                if (current_value_index){  
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                tt = IO_OUT_A;
                i++;
            }
            else if (current_char == '>'){
                if (current_value_index){                
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                tt = IO_OUT;
            }
            else if (current_char == '<'){                
                if (current_value_index){
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                tt = IO_IN;
            }
            else if (starts_with(line + i, "&&")){
                if (current_value_index){
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                i++;
                add_token = 1;
                tt = LOGIC_AND;
            }
            else if (starts_with(line + i, "||")){
                if (current_value_index){
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                i++;
                add_token = 1;
                tt = LOGIC_OR;
            }
            else if (current_char == '|'){                
                if (current_value_index){
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                tt = PIPE;
            }
            else if (starts_with(line + i, "if") && (i + 2 > len || !is_letter(line[i + 2]))){
                if (current_value_index){                
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                i += 1;
                tt = COND_IF;
            }
            else if (starts_with(line + i, "then") && (i + 4 > len || !is_letter(line[i + 4]))){
                if (current_value_index){                
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                i += 3;
                tt = COND_THEN;
            }
            else if (starts_with(line + i, "else") && (i + 4 > len || !is_letter(line[i + 4]))){
                if (current_value_index){                
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                i += 3;
                tt = COND_ELSE;
            }
            else if (starts_with(line + i, "end") && (i + 3 > len || !is_letter(line[i + 3]))){
                if (current_value_index){                
                    tokens[token_index++] = get_token(current_token_value, current_value_index, tt);
                }
                add_token = 1;
                i += 2;
                tt = COND_END;
            }
            else{
                current_token_value[current_value_index++] = line[i];                
            }
        }

        if (add_token){
            add_token = 0;
            token_t* t = get_token(current_token_value, current_value_index, tt);
            tokens[token_index++] = t;
            tt = 0;
        }
    }    

    if (current_value_index){
        token_t* t = get_token(current_token_value, current_value_index, tt);
        tokens[token_index++] = t;
    }

    if (on_string){
        
    }
    *tokens_count = token_index;
    // print_tokens(tokens, token_index);
    return tokens;
}

char* names[12] = {
    "TEXT",
    "IO_OUT",
    "IO_OUT_A",
    "IO_IN",
    "LOGIC_AND",
    "LOGIC_OR",
    "PIPE",
    "SEMICOLON",
    "COND_IF",
    "COND_ELSE",
    "COND_THEN",
    "COND_END"
};

void print_tokens(token_t** tokens, int count){
    for (int i = 0; i < count; i++)
    {
        token_t* t = tokens[i];
        print("[%s] ", names[t->type]);
        if (tokens[i]->type == TEXT){
            print("%s\n", tokens[i]->value);
        }
        else{
            print("\n");
        }
    }
}

char* repr[12] = {
    0, // TEXT
    0, // IO_OUT
    0, // IO_OUT_A
    0, // IO_IN
    0, // LOGIC_AND
    0, // LOGIC_OR
    0, // PIPE
    0, // SEMICOLON
    "if", // COND_IF
    "else", // COND_ELSE
    "then", // COND_THEN
    "end" // COND_END
};

char* str_repr[12] = {
    0, // TEXT
    ">", // IO_OUT
    ">>", // IO_OUT_A
    "<", // IO_IN
    "&&", // LOGIC_AND
    "||", // LOGIC_OR
    "|", // PIPE
    ";", // SEMICOLON
    "if", // COND_IF
    "else", // COND_ELSE
    "then", // COND_THEN
    "end" // COND_END
};

int is_symbol_token(token_t* token){
    return token->type > 0 && token->type < 8;
}

char* get_token_repr(token_t* token){
    if (token->type == TEXT){
        return token->value;
    }
    return repr[token->type];
}

char* get_tokens_repr(token_t** tokens, int count){
    char* line_repr = (char*)calloc(count * 100, sizeof(char));
    int index = 0;
    for (int i = 0; i < count; i++)
    {
        char* token_repr = tokens[i]->type == TEXT ? tokens[i]->value : str_repr[tokens[i]->type];
        int repr_len = strlen(token_repr);
        for (int j = 0; j < repr_len; j++){
            line_repr[index++] = token_repr[j];
        }
        line_repr[index++] = ' ';        
    }
    line_repr[index] = 0;        
    return line_repr;
}