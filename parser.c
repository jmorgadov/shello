#include "parser.h"
#include <stdlib.h>

char* temp_buff;

int count_char(char* str, char c){
    int count = 0;
    int i = 1;
    while (str[++i]){
        if (str[i] == c)
            count++;
    }
    return count;    
}

char** split(char* str, int* tokens_count){
    int len = strlen(str) + 1;
    temp_buff = (char*)malloc(len*sizeof(char));
    char** answ = (char**)malloc((count_char(str, ' ') + 1)*sizeof(char*));
    int j = 0;
    int a = 0;
    int i = 0;
    int count = 0;
    while (str[i]){
        if (str[i] == '\\' && str[i + 1]){
            switch (str[i + 1])
            {
                case ' ':    
                    temp_buff[j++] = ' ';
                    i++;            
                    break;            
                default:
                    break;
            }
        }
        else if (str[i] == ' '){
            temp_buff[j++] = 0;
            answ[a++] = temp_buff;
            temp_buff = (char*)malloc(len*sizeof(char));
            j = 0; 
            count++;
        }
        else{
            temp_buff[j++] = str[i];
        }
        i++;
    }
    temp_buff[j++] = 0;
    answ[a++] = temp_buff;
    count++;
    *tokens_count = count;
    return answ;
}