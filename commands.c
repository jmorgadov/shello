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
#include "strtools.h"
#include "string.h"
#include "datastructs.h"
#include "help.h"

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

int execute(command_t* command){   
    if (COMMAND_IS_("cd")){
        chdir(command->args[1]);
        return 1;
    }
    else if (COMMAND_IS_("exit")){
        exit(0);
    }    
    else if (COMMAND_IS_("history")){
        print_history_lines(history);
        return 1;
        // char** h_lines = get_history_lines(history);
        // int max = history->count;
        // int begin = max < HISTORY_MAX_SIZE ? 0 : history->index;
        // for (int i = 0; i < max; i++) {
        //     print("[%d] %s\n", i + 1, history->lines[(i + begin)%HISTORY_MAX_SIZE]);
        // }        
    }
    else if (COMMAND_IS_("true")){
        return 1;
    }
    else if (COMMAND_IS_("false")){
        return 0;
    }
    else if (COMMAND_IS_("help")){
        show_command_help(command->args[1]);
        return 1;
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


            if (command->p_out != 1)
                close(command->p_out);
            if (command->p_in != 0)
                close(command->p_in);  
            return status == 0;
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

            if (command->p_out != 1)
                close(command->p_out);
            if (command->p_in != 0)
                close(command->p_in);      
            exit(0);
        }
    }    
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

int execute_command_line(char** command_tokens, int tokens_count, char* line){
    if (tokens_count == 0)
        return 1;
    char** temp_command = (char**)malloc(tokens_count*sizeof(char*));
    command_t** commands = (command_t**)malloc(MAX_COMMAND_IN_LINE*sizeof(command_t*));

    // Dividing commands by pipes
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

    //check again command
    if (STR_EQ(commands[0]->args[0], "again"))
    {
        int token_counts = 0;
        char* command_line = get_at(atoi(commands[0]->args[1]) - 1, history);
        if (command_line == NULL)
            return 1;
        process_line(command_line);
        return 1;
    }

    resolve_pipes(commands, c);
    int return_val = 0;
    for (int i = 0; i < c; i++)
    {        
        command_t* comm = commands[i];
        return_val = execute(comm);
    }  
    return return_val;
}

int parse_and_execute(char* line){
    int cmd_token_counts = 0;
    char** cmd_tokens = split(line, &cmd_token_counts);
    return execute_command_line(cmd_tokens, cmd_token_counts, line);
}

void execute_command_stack(my_stack_t* command_stack, my_stack_t* ops)
{
    for (int i = 0; i < ops->items->count; i++)
    {
        char* current_op = (char*)pop(ops);    
        if (current_op != NULL){
            if (STR_EQ(current_op, "&&")){
                char* line = (char*)pop(command_stack);
                int return_val = parse_and_execute(line);
                if (return_val == 1)
                    continue;
                else
                    return;
            }
            else if (STR_EQ(current_op, "||")){
                char* line = (char*)pop(command_stack);
                int return_val = parse_and_execute(line);
                if (return_val == 0)
                    continue;
                else
                    return;
            }
        }        
    }
    if (command_stack->items->count > 0){
        char* line = (char*)pop(command_stack);
        int return_val = parse_and_execute(line);
    }
    
}

my_stack_t* generate_command_stack(char* command_line, my_stack_t* ops){
    my_stack_t* s = stackinit();
    my_stack_t* temp = stackinit();
    int token_counts = 0;
    char* split_tokens[2];
    split_tokens[0] = "&&";
    split_tokens[1] = "||";
    char** command_tokens = splitbyr(command_line, &token_counts, split_tokens, 2, 0);

    for (int i = token_counts - 1; i >= 0; i--)
    {
        char* token = command_tokens[i];
        if (STR_EQ(token, "&&") || STR_EQ(token, "||")){
            push(ops, (char*)token);
        }
        else{
            push(s, (char*)token);
        }
    }
    for (int i = 0; i < temp->items->count; i++){
        push(s, (char*)pop(temp));
    }
    return s;
}

char** parse_line(char* line, int* steps_count){
    char* parse_tokens[1];
    parse_tokens[0] = ";";
    char** commands = splitby(line, steps_count, parse_tokens, 1);
    return commands;
}

int is_digit(char c){
    return c >= '0' && c <= '9';
}

char* repace_again_commands(char* line)
{
    int len = strlen(line);
    char* newLine = (char*)malloc(sizeof(char)*500);
    char* number = (char*)malloc(sizeof(char)*10);
    int pos = 0;
    for (int i = 0; i < len; i++)
    {
        if (starts_with(line + i, "again")){
            int temp = i;
            temp += 5;
            while (line[temp] == ' ')
                temp++;
            int np = 0;
            while (is_digit(line[temp])){
                number[np++] = line[temp++];
            }
            number[np] = 0;
            char* hist_line = get_at(atoi(number) - 1, history);
            int hp = 0;
            while (hist_line[hp] != '\n' && hist_line[hp] != 0){
                newLine[pos++] = hist_line[hp++];
            }
            newLine[pos++] = ' ';
            i = temp;
        }
        else{
            newLine[pos++] = line[i];
        }
    }
    newLine[pos] = 0;
    return newLine;
    
}

void process_line(char* line){
    if (history == NULL)
        history = init_history_handler();

    char* newline = repace_again_commands(line);

    // Separate execution steps by semicolon (;)
    int steps_count = 0;
    char** command_lines = parse_line(newline, &steps_count);

    // Generate the stack and run the command of each command line 
    for (int i = 0; i < steps_count; i++){
        my_stack_t* ops = stackinit();
        my_stack_t* current_command_stack = generate_command_stack(command_lines[i], ops);
        execute_command_stack(current_command_stack, ops);
    }    
    add_line(newline, history);
}