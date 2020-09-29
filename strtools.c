#include "strtools.h"
#include <stdlib.h>
#include <string.h>
#include "debug.h"

char *temp_buff;

#define FALSE 0
#define TRUE 1
#define NOT(value) value == 0

typedef int bool;

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

char** splitbyr(char* str, int* tokens_count, char** splitItems, int splitItemsCount, int removeSplitItems){
    int len = strlen(str) + 1;
    temp_buff = (char *)malloc(len * sizeof(char));
    int t_count = 0;
    char **answ = (char **)malloc(50 * sizeof(char *));

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