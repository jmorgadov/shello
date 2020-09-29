#ifndef PARSER_H
#define PARSER_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>

char** split(char* str, int* tokens_count);
char** splitby(char* str, int* tokens_count, char splitChar);

#endif