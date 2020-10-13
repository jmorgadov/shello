#ifndef STRTOOLS_H
#define STRTOOLS_H

int starts_with(char* str, char* comp);
char** split(char* str, int* tokens_count);
char** splitby(char* str, int* tokens_count, char** splitItems, int splitItemsCount);
char** splitbyr(char* str, int* tokens_count, char** splitItems, int splitItemsCount, int removeSplitItems);
int first_occurrense(char *str, char **values, int values_count, int check_inside_strings, int* value_index);
char* remove_initial_spaces(char* str);
char* remove_unnecesary_spaces(char *line);
char* replace(char* str, char* old, char* new);
char* remove_str_repr(char* str);
#endif