#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "commands.h"
#include "debug.h"
#include "history.h"

typedef int bool;

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0
#define COMMAND_IS_(s) STR_EQ(command->name, s)

#define MAX_COMMAND_IN_LINE 10

history_h* history;

command_t* init_command(char* name, char** args){
    command_t* comm = (command_t*)malloc(sizeof(command_t));
    comm->name = name;
    comm->args = args;
    comm->in = NULL;
    comm->out = NULL;  
    comm->p_in = 0;
    comm->p_out = 1;  
    return comm;
}

void resolve_pipes(command_t** commands, int len){
    for (int i = 1; i < len; i++)
    {
        int p[2];
        pipe(p);
        commands[i-1]->p_out = p[1];
        commands[i]->p_in = p[0];
    }    
}

void execute(command_t* command){   
    if (history == NULL)
        history = init_history_handler();
    if (COMMAND_IS_("cd")){
        chdir(command->args[1]);
    }
    else if (COMMAND_IS_("exit")){
        exit(0);
    }    
    else if (COMMAND_IS_("history")){        
        char** h_lines = get_history_lines(history);
        int max = history->count < HISTORY_MAX_SIZE ? history-> count : HISTORY_MAX_SIZE;
        for (int i = 0; i < max; i++) {
            print("[%d] %s\n", i + 1, h_lines[i]);
        }        
    }
    else
    {
        int child_pid = 0;
        int status = 0;
        if (child_pid = fork()){
            waitpid(child_pid, &status, W_OK);
            FILE* out = command->out;
            FILE* in = command->in;
            if (out->_fileno != 0 && out->_fileno != 1){
                fclose(out);
            }        
            if (in->_fileno != 0 && in->_fileno != 1){
                fclose(in);                
            } 
            // wait(&status);
        }
        else{
            int i = 0;
            int o = 1;
            if (command->p_in != 0)
                i = dup2(command->p_in, 0);
            else
                i = dup2(command->in->_fileno, 0);

            if (command->p_out != 1)
                o = dup2(command->p_out, 1);
            else
                o = dup2(command->out->_fileno, 1);

            if (i != 0)
                close(i);
            if (o != 1)
                close(o);
            
            
            execvp(command->name, command->args);  
            FILE* out = command->out;
            FILE* in = command->in;
            if (out->_fileno && out->_fileno != 1){
                fclose(out);                
            }        
            if (in->_fileno){
                fclose(in);                
            }            
            exit(0);
        }
    }    
    add_line(command, history);
}

char** resolve_files_in_out(char** commands, int len, FILE** in, FILE** out){
    FILE* infd = stdin;
    FILE* outfd = stdout;
    char** answ = (char**)malloc(len*sizeof(char*));
    int answ_index = 0;
    for (int i = 0; i < len; i++)
    {
        if (STR_EQ(commands[i], ">")){
            char* path = commands[i + 1];
            i++;
            FILE* fd = fopen(path, "w+");
            if (fd->_fileno < 2){
                printc(RED, "Error creating file %s\n", path);
            }
            else{
                outfd = fd;
            }
        }
        else if (STR_EQ(commands[i], ">>")){
            char* path = commands[i + 1];
            i++;
            FILE* fd = fopen(path, "a+");
            if (fd->_fileno < 2){
                printc(RED, "Error opening file %s\n", path);
            }
            else{
                outfd = fd;
            }
        }
        else if (STR_EQ(commands[i], "<")){
            char* path = commands[i + 1];
            i++;
            FILE* fd = fopen(path, "r");
            if (fd->_fileno < 2){
                printc(RED, "Error opening file %s\n", path);
            }
            else{
                infd = fd;
            }
        }
        else{
            answ[answ_index++] = commands[i];
        }
    }    
    char** final_answ = (char**)malloc(answ_index*sizeof(char*));
    for (int i = 0; i < answ_index; i++)
        final_answ[i] = answ[i];

    if (infd->_fileno >= 0)
        *in = infd;
    if (outfd->_fileno >= 0)
        *out = outfd;
    return final_answ;    
}

command_t* get_command(char** command, int len){
    FILE* in = stdin;
    FILE* out = stdout;
    char** comm = resolve_files_in_out(command, len, &in, &out);
    command_t* temp = init_command(comm[0], comm);
    temp->in = in;
    temp->out = out;
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
            commands[c++] = get_command(temp_command, k);
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