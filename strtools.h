#ifndef STRTOOLS_H
#define STRTOOLS_H

int starts_with(char* str, char* comp);
char** split(char* str, int* tokens_count);
char** splitby(char* str, int* tokens_count, char** splitItems, int splitItemsCount);
char** splitbyr(char* str, int* tokens_count, char** splitItems, int splitItemsCount, int removeSplitItems);
#endif