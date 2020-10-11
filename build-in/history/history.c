#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readf(FILE *file, int *__lines_count) { 
    int size = 1000;
    int lines = 1;
    char* text = (char*)malloc(sizeof(char)*size);
    int i = 0;
    while (i < size) {   
        char b = fgetc(file);
        text[i++] = b == EOF ? 0 : b;
        lines += b == 10 ? 1 : 0;
    }
    if (text[i - 1] == 10)
        lines--;
    *__lines_count = lines;
    return text;
}

int main(int argc, char** args){
    FILE* hist = fopen("/shello_cmd_history", "r");
    int linesCount = 0;
    char* text = readf(hist, &linesCount);
    char* line;
    for (int i = 0; i < linesCount - 1; i++) {        
        line = strtok(i == 0 ? text : NULL, "\n");
        printf("[%d] %s\n", i + 1, line);
    }
    fclose(hist);
    return 0;
}