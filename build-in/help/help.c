#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0

int main(int argc, char **args){
    FILE* f = (FILE*)malloc(sizeof(FILE));
    char* buff = (char*)malloc(sizeof(char));

    char* path = (char*)calloc(100, sizeof(char));
    strcpy(path, args[1]);
    strcat(path, "/build-in/help/help_files/");
    if (argc <= 2 || STR_EQ(args[2], "help")){
        strcat(path, "intro");
    }
    else if (STR_EQ(args[2], "true")){
        strcat(path, "true");
    }
    else if (STR_EQ(args[2], "false")){
        strcat(path, "false");
    }
    else if (STR_EQ(args[2], "history")){
        strcat(path, "history");
    }
    else if (STR_EQ(args[2], "again")){
        strcat(path, "again");
    }
    else if (STR_EQ(args[2], "impl")){
        strcat(path, "impl");
    }
    else if (STR_EQ(args[2], "multi-pipe")){
        strcat(path, "multi_pipe");
    }
    else if (STR_EQ(args[2], "spaces")){
        strcat(path, "spaces");
    }
    else if (STR_EQ(args[2], "chain")){
        strcat(path, "chain");
    }
    else if (STR_EQ(args[2], "basic")){
        strcat(path, "basic");
    }
    else if (STR_EQ(args[2], "if") || STR_EQ(args[2], "then") || STR_EQ(args[2], "else") || STR_EQ(args[2], "end") || STR_EQ(args[2], "multi-if")){
        strcat(path, "if");
    }
    else{
        printf("'%s' it's not a shello command\n\n", args[2]);
        return 0;
    }

    f = fopen(path, "r");
    while (read(f->_fileno,buff,1) > 0){
        printf("%c", *buff);
    }
    printf("\n\n");
    fclose(f);
    return 0;
}