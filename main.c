#include "commands.h"
#include "debug.h"
#include "parser.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define TRUE 1
#define FALSE 0
#define SHELL_NAME "mishell"
#define LINE_BUFF_SIZE 100 

typedef char* string;

char cwd[100];
char* line_buff;

string readf(FILE *file, int size) { 

    string text = (char*)malloc(size*sizeof(char));
    int i = 0;
    while (i < size) {   
        char b = fgetc(file);
        text[i++] = b == EOF ? 0 : b;
    }
    return text;
}

void printPrompt(){
    printc(BOLD_GREEN, "%s", SHELL_NAME);
    print(":");
    printc(BOLD_BLUE, "%s", cwd);
    print("$");
}

void printIntro(){
    int f = open("intro.txt", O_RDONLY);
    char* buff = (char*)malloc(sizeof(char));
    while (read(f,buff,1) > 0)
        print("%d", *buff);
    print("\n");
    close(f);
}

int main(int agrc, char **args)
{
    setbuf(stdout, NULL);
    // printIntro();
    line_buff = (char*)malloc(LINE_BUFF_SIZE * sizeof(char));
    while (TRUE)
    {
        getcwd(cwd, 100);
        printPrompt();
        int read_count = read(STDIN_FILENO, line_buff, LINE_BUFF_SIZE);
        line_buff[read_count - 1] = 0;
        char** line = split(line_buff);
        execute(line[0], line);
    }    
}