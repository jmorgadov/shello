#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "commands.h"
#include "debug.h"

typedef int bool;

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0
#define COMMAND_IS_(s) STR_EQ(command->name, s)

#define MAX_COMMAND_IN_LINE 10

command_t* init_command(char* name, char** args){
    command_t* comm = (command_t*)malloc(sizeof(command_t));
    comm->name = name;
    comm->args = args;
    comm->in = 0;
    comm->out = 1;  
    return comm;
}

void resolve_pipes(command_t** commands, int len){
    for (int i = 1; i < len; i++)
    {
        int p[2];
        pipe(p);
        commands[i-1]->out = p[1];
        commands[i]->in = p[0];
    }    
}


void execute(command_t* command){    
    if (COMMAND_IS_("cd")){
        chdir(command->args[1]);
    }
    else if (COMMAND_IS_("exit")){
        exit(0);
    }    
    else
    {
        int child_pid = 0;
        int status = 0;
        if (child_pid = fork()){
            waitpid(child_pid, &status,W_OK);
            int out = command->out;
            if (out != 0 && out != 1){
                close(out);
            }
            // wait(&status);
        }
        else{
            setbuf(stdout, NULL);
            dup2(command->in, 0);
            dup2(command->out, 1);
            execvp(command->name, command->args);  
            int out = command->out;
            if (out != 0 && out != 1){
                close(out);
            }      
            exit(0);
        }
    }    
}

command_t* get_command(char** command){
    command_t* temp = init_command(command[0], command);
    return temp;
}

void execute_line(char** command_tokens, int tokens_count){
    if (tokens_count == 0)
        return;
    char** temp_command = (char**)malloc(tokens_count*sizeof(char*));
    command_t** commands = (command_t**)malloc(MAX_COMMAND_IN_LINE*sizeof(command_t*));
    int k = 0;
    int c = 0;
    for (int i = 0; i <= tokens_count; i++){
        if (i == tokens_count || STR_EQ(command_tokens[i], "|")) {
            if (c == MAX_COMMAND_IN_LINE)
            {
                printc(RED, "Max command in line exceded\n");
                exit(0);
            }
            commands[c++] = get_command(temp_command);
            k = 0;
        }
        else{
            temp_command[k++] = command_tokens[i];
        }
    }
    resolve_pipes(commands, c);
    for (int i = 0; i < c; i++)
    {        
        command_t* comm = commands[i];
        execute(comm);
    }  
    
}