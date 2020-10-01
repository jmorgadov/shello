#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0

void show_command_help(char* command){
    FILE* f = (FILE*)malloc(sizeof(FILE));
    char* buff = (char*)malloc(sizeof(char));    

    if (command == NULL || strlen(command) == 0){        
        f = fopen("./help_files/intro", "r");
    }
    else if (STR_EQ(command, "true")){
        f = fopen("./help_files/true", "r");
    }
    else if (STR_EQ(command, "false")){
        f = fopen("./help_files/false", "r");
    }
    else if (STR_EQ(command, "history")){
        f = fopen("./help_files/history", "r");
    }
    else if (STR_EQ(command, "again")){
        f = fopen("./help_files/again", "r");
    }
    else{
        printf("%s it's not a shello command\n\n", command);
        free(f);
        return;
    }

    while (read(f->_fileno,buff,1) > 0)
        printf("%c", *buff);
    printf("\n\n");
    fclose(f);  
    free(f);
}