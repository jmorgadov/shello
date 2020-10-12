#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0

int main(int argc, char **args){
    FILE* f = (FILE*)malloc(sizeof(FILE));
    char* buff = (char*)malloc(sizeof(char));

    if (argc <= 1){
        f = fopen("./build-in/help/help_files/intro", "r");
    }
    else if (STR_EQ(args[1], "true")){
        f = fopen("./build-in/help/help_files/true", "r");
    }
    else if (STR_EQ(args[1], "false")){
        f = fopen("./build-in/help/help_files/false", "r");
    }
    else if (STR_EQ(args[1], "history")){
        f = fopen("./build-in/help/help_files/history", "r");
    }
    else if (STR_EQ(args[1], "again")){
        f = fopen("./build-in/help/help_files/again", "r");
    }
    else{
        printf("'%s' it's not a shello command\n\n", args[1]);
        free(f);
        return 0;
    }

    while (read(f->_fileno,buff,1) > 0){
        printf("%c", *buff);
    }
    printf("\n\n");
    fclose(f);
    return 0;
}