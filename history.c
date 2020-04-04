#include "commands.h"
#include "history.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define LINE_BUFF_SIZE 500 

history_h* init_history_handler(){
    history_h* hh =(history_h*) malloc(sizeof(history_h));
    hh->count = 0;
    hh->index = 0;
    hh->lines = (char**)malloc(HISTORY_MAX_SIZE*sizeof(char));
    return hh;
}

int add_line (command_t* comand,history_h* hh){
    hh->count++;
    hh->lines[hh->index] = comand->args[0];
    hh->index= (hh->index+1)%HISTORY_MAX_SIZE;
    return 0;
}

char** get_history_lines (history_h* hh){
    int c = hh->count;
    int index = hh->index;
    char** lines = (char**)malloc(HISTORY_MAX_SIZE*sizeof(char*));    

    if (c < HISTORY_MAX_SIZE)
        for (int i = 0; i < c; i++)
           lines[i]=hh->lines[i]; 
    else
    {
        int indx = hh->index;
        for (int i = 0; i < HISTORY_MAX_SIZE; i++)
            lines[i] = hh->lines[(index + i)%HISTORY_MAX_SIZE];
    }
    return lines;
}


