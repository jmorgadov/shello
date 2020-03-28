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
    print("init command\n");
    command_t* comm = (command_t*)malloc(sizeof(command_t));
    comm->name = name;
    comm->args = args;
    comm->in = 0;
    comm->out = 1;  
    print("end init command  %s --\n", name);
    return comm;
}

void resolve_pipes(command_t** commands, int len){
    print("Resolving %d pipes\n", len);
    for (int i = 1; i < len; i++)
    {
        int p[2];
        pipe(p);
        commands[i-1]->out = p[1];
        commands[i]->in = p[0];
    }    
    print("end resolving pipes\n");
}


void execute(command_t* command){    
    print("executing command\n");
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
            // waitpid(child_pid, &status, WNOHANG);
            wait(&status);
        }
        else{
            dup2(command->in, 0);
            dup2(command->out, 1);
            execvp(command->name, command->args);            
            exit(0);
        }
    }    
}

command_t* get_command(char** command){
    print("getting command\n");
    command_t* temp = init_command(command[0], command + 1);
    print("name --- %d\n", temp->out);
    return temp;
}

void execute_line(char** command_tokens, int tokens_count){
    if (tokens_count == 0)
        return;
    char** temp_command = (char**)malloc(tokens_count*sizeof(char*));
    command_t** commands = (command_t**)malloc(MAX_COMMAND_IN_LINE*sizeof(command_t*));
    int k = 0;
    int c = 0;
    printc(RED, "Tokens count: %d\n", tokens_count);
    for (int i = 0; i <= tokens_count; i++){
        printc(RED, "%d\n", i);
        if (i == tokens_count || STR_EQ(command_tokens[i], "|")) {
            if (c == MAX_COMMAND_IN_LINE)
            {
                printc(RED, "Max command in line exceded\n");
                exit(0);
            }
            printc(RED, "   %d\n",k);
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
        print("name = %s\n", comm->name);
        execute(comm);
    }
    
    
}