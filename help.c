#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0

void show_command_help(char* command){
    if (command == NULL || strlen(command = 0)){
        FILE* f = fopen("./help_files/intro", "r");
        char* buff = (char*)malloc(sizeof(char));    
        while (read(f->_fileno,buff,1) > 0)
            printf("%c", *buff);
        printf("\n");
        fclose(f);  
    }
}