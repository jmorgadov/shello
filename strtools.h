#ifndef STRTOOLS_H
#define STRTOOLS_H

char** split(char* str, int* tokens_count);
char** splitby(char* str, int* tokens_count, char** splitItems, int splitItemsCount);
char** splitbyr(char* str, int* tokens_count, char** splitItems, int splitItemsCount, int removeSplitItems);
#endif