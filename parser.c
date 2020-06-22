#include "parser.h"
#include <stdlib.h>
#include "debug.h"

char* temp_buff;

#define FALSE 0
#define TRUE 1
#define NOT(value) value == 0

typedef int bool;

int count_line_tokens(char* str){
    int count = 0;
    int i = 0;
    bool on_string = FALSE;

    // move to first char
    while (str[i] == ' ') { i++; }
    
    char last = str[i];

    while (str[++i]){
        if ((str[i] == '\'' || str[i] == '\"') && last != '\\')
        {
            if (on_string)
                on_string = FALSE;
            else
                on_string = TRUE;
        }
        if (NOT(on_string) && str[i] == ' ' && last != ' ')
            count++;

        last = str[i];
    }
    if (str[i - 1] == ' ')
        count--;
    return count + 1;    
}

char** split(char* str, int* tokens_count){
    print("\n\n");
    int len = strlen(str) + 1;
    temp_buff = (char*)malloc(len*sizeof(char));
    int t_count = count_line_tokens(str);
    char** answ = (char**)malloc(t_count*sizeof(char*));

    int tb_idx = 0;
    int a_idx = 0;
    int i = 0;    

    char last = 0;
    bool on_string = FALSE;

    while (str[i] == ' ') { i++; }

    while (str[i] && str[i] != '#')
    {        
        char current = str[i];

        if (last == ' ')
        {
            while (str[i] == ' ') { i++; }
            last = 0;
            continue;
        }
        if ((current == '\'' || current == '\"') && last != '\\')
        {
            if (on_string)
                on_string = FALSE;
            else
                on_string = TRUE;
            
            temp_buff[tb_idx++] = current;
        }
        else if(current == ' ')
        {
            if (on_string)
                temp_buff[tb_idx++] = current;            
            else
            {
                temp_buff[tb_idx] = 0;
                answ[a_idx++] = temp_buff;
                temp_buff = (char*)malloc(len*sizeof(char));
                tb_idx = 0;
            }            
        }
        else
        {
            temp_buff[tb_idx++] = current;
        }
        last = str[i];

        i++;
    }        
    if (tb_idx != 0) 
    {
        temp_buff[tb_idx] = 0;
        answ[a_idx++] = temp_buff;
    }
    *tokens_count = t_count; 
    print("\ntokens: %d\n\n", t_count);   
    return answ;
}