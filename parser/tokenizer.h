#ifndef TOKENIZER_H
#define TOKENIZER_H

enum token_type{
    TEXT,
    IO_OUT,
    IO_OUT_A,
    IO_IN,
    LOGIC_AND,
    LOGIC_OR,
    PIPE,
    SEMICOLON,
    COND_IF,
    COND_ELSE,
    COND_THEN,
    COND_END
};

typedef struct token{
    int type;
    char* value;
}token_t;

token_t** get_tokens(char* line, int* tokens_count, int* error);
void print_tokens(token_t** tokens, int count);
int is_symbol_token(token_t* token);
char* get_token_repr(token_t* token);
char* get_tokens_repr(token_t** tokens, int count);

#endif