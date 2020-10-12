#include "strtools.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"

char *temp_buff;

#define FALSE 0
#define TRUE 1
#define NOT(value) value == 0

typedef int bool;

char* remove_initial_spaces(char* str){
    int i = 0;
    int len = strlen(str);
    while (i < len && str[i] == ' ')
        i++;
    return str + 1;
}

int starts_with_any(char* str, char** values, int values_count){
    for (int i = 0; i < values_count; i++)
    {
        int j = 0;
        int match = TRUE;
        while(values[i][j]){
            if (!str[j]) return -1;
            if (values[i][j] != str[j]){
                match = FALSE;                
                break;
            }
            j++;
        }
        if (match) return i;
    }
    return -1;
}

int first_occurrense(char *str, char **values, int values_count, int check_inside_strings, int* value_index){
    int is_inside_string = 0;
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (!check_inside_strings && str[i] == '\"' && i > 0 && str[i - 1] != '\\'){
            if(is_inside_string)
                is_inside_string = 0;
            else
                is_inside_string = 1;
            continue;
        }
        if (!is_inside_string){
            int index = starts_with_any(str + i, values, values_count);
            if (index != -1){
                *value_index = index;
                return i;
            }
        }
    }
    return -1;
}

int starts_with(char* str, char* comp)
{
    if (strlen(comp) > strlen(str)) return 0;

    char* split_tokens[1];
    split_tokens[0] = (char*)malloc(sizeof(char)*(strlen(comp) + 1));
    strcpy(split_tokens[0], comp);
    return starts_with_any(str, split_tokens, 1) != -1;
}

char** splitbyr(char* str, int* tokens_count, char** splitItems, int splitItemsCount, int removeSplitItems){
    int len = strlen(str) + 1;
    temp_buff = (char *)malloc(len * sizeof(char));
    int t_count = 0;
    char **answ = (char **)calloc(300,sizeof(char *));

    int tb_idx = 0;
    int a_idx = 0;
    int i = 0;

    char last = 0;
    bool on_string = FALSE;

    while (str[i] == ' '){
        i++;
    }

    while (str[i] && str[i] != '#'){
        char current = str[i];
        int splitMatchIndex = -1;
        if (last == ' '){
            while (str[i] == ' '){
                i++;
            }
            last = 0;
            continue;
        }
        if ((current == '\'' || current == '\"') && last != '\\'){
            if (on_string)
                on_string = FALSE;
            else
                on_string = TRUE;

            temp_buff[tb_idx++] = current;
        }
        else if ((splitMatchIndex = starts_with_any(str + i, splitItems, splitItemsCount)) != -1){
            if (on_string)
                temp_buff[tb_idx++] = current;
            else{
                temp_buff[tb_idx] = 0;
                answ[a_idx++] = temp_buff;
                temp_buff = (char *)malloc(len * sizeof(char));
                tb_idx = 0;
                t_count++;

                if (!removeSplitItems){
                    temp_buff[tb_idx] = 0;
                    strcpy(temp_buff, splitItems[splitMatchIndex]);
                    answ[a_idx++] = temp_buff;
                    temp_buff = (char *)malloc(len * sizeof(char));
                    tb_idx = 0;
                    t_count++;
                }
                i += strlen(splitItems[splitMatchIndex]) - 1;
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
        t_count++;
    }
    *tokens_count = t_count;
    return answ;
}
char** splitby(char* str, int* tokens_count, char** splitItems, int splitItemsCount){
    return splitbyr(str, tokens_count, splitItems, splitItemsCount, 1);
}

char **split(char *str, int *tokens_count)
{
    char** default_split = (char**)malloc(sizeof(char*));
    default_split[0] = " ";
    return splitby(str, tokens_count, default_split, 1);
}

char* remove_unnecesary_spaces(char *line){
    int len = strlen(line);
    char* answ = (char*)malloc(sizeof(char)*len);
    int answ_index = 0;
    char last = ' ';
    for (int i = 0; i < len; i++)
    {
        if (line[i] == ' ' && last == ' ')
            continue;
        answ[answ_index++] = line[i];
        last = line[i];
    }
    answ[answ_index] = 0;
    return answ;   
}

int count_occurrencies(char* str, char* item){
    int item_len = strlen(item);
    int len = strlen(str);
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        if (starts_with(str + i, item)){
            count++;
            i += item_len - 1;
        }
    }
    
}

char* replace(char* str, char* old, char* new){
    int new_len = strlen(new);
    int old_len = strlen(old);
    int len = strlen(str);
    int answ_len = len + count_occurrencies(str, old) * (new_len - old_len);
    char* answ = (char*)malloc(sizeof(char)*(answ_len + 1));
    int answ_index = 0;
    for (int i = 0; i < len; i++)
    {
        if (starts_with(str + i, old)){
            strcpy(answ + answ_index, new);
            answ_index += new_len;
        }
        else{
            answ[answ_index++] = str[i];
        }
    }
    answ[answ_index] = 0;
    return answ;
}