/*
Functionalities status:

   No. Name           Status
- (01) Basic -------- OK
- (02) Basic -------- OK
- (03) Basic -------- OK
- (04) Basic -------- OK
- (05) Basic -------- OK
- (06) Basic -------- OK
- (07) Basic -------- OK
- (08) Basic -------- OK
- (09) Multi-pipe --- OK
- (10) Background ---
- (11) Spaces ------- OK
- (12) History ------ OK
- (13) Ctrl+C -------
- (14) Chain -------- OK
- (15) If ----------- OK
- (16) Multi-If ----- OK
- (17) Help --------- OK
- (18) Variables ----
*/

#include "commands.h"
#include "debug.h"
#include "./parser/parser.h"
#include "./parser/ast_structs.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define SHELL_NAME "shello"
#define LINE_BUFF_SIZE 500 
#define CLEAN_BUFFER_SIZE 200

#define UP_ARROW_KEY "[A"
#define DOWN_ARROW_KEY "[B"
#define RIGHT_ARROW_KEY "[C"
#define LEFT_ARROW_KEY "[D"



typedef char* string;

char cwd[500];
char* line_buff;

void printPrompt(){
    printc(BOLD_GREEN, "%s", SHELL_NAME);
    print(":");
    printc(BOLD_BLUE, "%s", cwd);
    print("$ ");
}

void printIntro(){
    FILE* f = fopen("intro", "r");
    char* buff = (char*)malloc(sizeof(char));    
    while (read(f->_fileno,buff,1) > 0)
        print("%c", *buff);
    print("\n");
    fclose(f);    
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
            perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
    return (buf);
}

int ends_with(char* a, char* b){
    int lena = strlen(a);
    int lenb = strlen(b);
    if (lenb > lena)
        return FALSE;
    return strcmp(a + (lena - lenb), b) == 0;
}

void reset_line(int len){
    for (int i = 0; i < len; i++)
        print("\b \b");  
}

int main(int argc, char **args)
{

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    printIntro();
    line_buff = (char*)malloc(LINE_BUFF_SIZE * sizeof(char));
    int buffer_index = 0;
    char current = 0;
    int reading = TRUE;
    getcwd(cwd, 500);
    set_init_path(strdup(cwd));
    while (TRUE)
    {
        getcwd(cwd, 500);
        printPrompt();
        reading = TRUE;
        buffer_index = 0;        
        while (reading){
            switch (current = getch())
            {
                case '\n':                    
                    print("%c", current);
                    reading = FALSE;
                    break;
                case 9:
                    reading = FALSE;
                case 127:
                    if (buffer_index){
                        print("\b \b"); 
                        buffer_index--;
                    }
                    break;
                default:
                    line_buff[buffer_index++] = current;
                    print("%c", current);
                    break;
            }
            if (ends_with(line_buff, UP_ARROW_KEY) || current == 9){
                // free(line_buff);
                line_buff = (char*)malloc(LINE_BUFF_SIZE*sizeof(char));
                reset_line(buffer_index);
                buffer_index = 0;             
            }
        }


        if (buffer_index == 0)
            continue;
            
        line_buff[buffer_index++] = ' ';
        line_buff[buffer_index] = 0;
        execute_shell_line(line_buff);       
    }    

}