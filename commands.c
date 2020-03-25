#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "commands.h"


typedef char* string;
typedef int bool;

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0
#define COMMAND_IS_(s) STR_EQ(command->name, s)

command_t* init_command(string name, string* args, CommandType type){
    command_t* comm = (command_t*)malloc(sizeof(command_t));
    comm->name = name;
    comm->args = args;
    comm->type = type;
}

void execute(command_t* command){    
    switch (command->type)
    {
        case shell:
            if (COMMAND_IS_("cd")){
                chdir(command->args[1]);
            }
            else if (COMMAND_IS_("exit")){
                exit(0);
            }       
            break;   

        case system:
            int child_pid = 0;
            int status = 0;
            if (child_pid = fork()){
                // waitpid(child_pid, &status, WNOHANG);
                wait(&status);
            }
            else{
                execvp(command->name, command->args);
                exit(0);
            }
            break;  

        case file:
            break;  

        case special:
            if (COMMAND_IS_("|")){
                
            }
            else if (COMMAND_IS_(">")){

            }
            else if (COMMAND_IS_("<")){

            }
            else if (COMMAND_IS_("&")){
                
            }
            break;   
    }    
}