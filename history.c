#include "debug.h"
#include "history.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define LINE_BUFF_SIZE 500 

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

history_h* init_history_handler(){
    history_h* hh =(history_h*) malloc(sizeof(history_h));
    
    FILE* hist1 = fopen("/shello_cmd_history", "a");
    fclose(hist1);
    FILE* hist2 = fopen("/shello_cmd_history", "rw");
    int linesCount = 0;
    char* text = readf(hist2, &linesCount);
    fclose(hist2);

    hh->count = linesCount > 0 ? linesCount - 1 : linesCount;
    hh->index = hh->count == HISTORY_MAX_SIZE ? 0 : hh->count;
    hh->lines = (char**)malloc(HISTORY_MAX_SIZE*sizeof(char*));

    char* line;
    for (int i = 0; i < hh->count; i++)
    {
        line = strtok(i == 0 ? text : NULL, "\n");
        hh->lines[i] = (char*)malloc(sizeof(char)*LINE_BUFF_SIZE);
        strcpy(hh->lines[i], line);
    }
    
    return hh;
}

int add_line (char* command, history_h* hh){
    hh->count = (hh->count+1) > HISTORY_MAX_SIZE ? HISTORY_MAX_SIZE : (hh->count+1);    
    hh->lines[hh->index] = (char*)malloc(sizeof(char)*LINE_BUFF_SIZE);
    strcpy(hh->lines[hh->index], command);
    hh->index = (hh->index+1)%HISTORY_MAX_SIZE;

    FILE* hist1 = fopen("/shello_cmd_history", "r");
    int linesCount = 0;
    char* text = readf(hist1, &linesCount);
    fclose(hist1);
    remove("/shello_cmd_history");

    FILE* hist = fopen("/shello_cmd_history", "w");
    char* line;
    for (int i = 0; i < linesCount - 1; i++) {   
        line = strtok(i == 0 ? text : NULL, "\n");
        if (i == 0 && hh->count == HISTORY_MAX_SIZE) continue;     
        fwrite(line, sizeof(char), strlen(line), hist);
        fwrite("\n", sizeof(char), 1, hist);
    }
    int current_pos = hh->index - 1;
    if (current_pos < 0)
        current_pos = hh->count - 1;
    fwrite(hh->lines[current_pos], sizeof(char), strlen(hh->lines[current_pos]), hist);
    fwrite("\n", sizeof(char), 1, hist);    
    fclose(hist);
    return 0;
}

void print_history_lines(history_h* hh){
    FILE* hist = fopen("/shello_cmd_history", "r");
    int linesCount = 0;
    char* text = readf(hist, &linesCount);
    char* line;
    for (int i = 0; i < linesCount - 1; i++) {        
        line = strtok(i == 0 ? text : NULL, "\n");
        printf("[%d] %s\n", i + 1, line);
    }
    fclose(hist);
}

char* get_at(int index, history_h* hh){
    if (index < 0 || index >= hh->count){
        printc(RED, "\nIndexOutOfRangeException at command lines\n");
        return NULL;
    }
    return hh->lines[(hh->index + index) % hh->count];
}


