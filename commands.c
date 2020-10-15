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
#include "datastructs.h"
#include "parser/ast_structs.h"
#include "parser/parser.h"

typedef int bool;

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0
#define COMMAND_IS_(s) STR_EQ(command->name, s)

#define MAX_COMMAND_IN_LINE 10

history_h* history = NULL;

int execute_command(command_t* command){
    if (COMMAND_IS_("cd")){
        chdir(command->args[1]);
        return 1;
    }
    else if (COMMAND_IS_("exit")){
        exit(0);
    }
    else if (COMMAND_IS_("true")){
        return 1;
    }
    else if (COMMAND_IS_("false")){
        return 0;
    }
    else
    {
        if (COMMAND_IS_("help")){
            command->name = "./build-in/help/help.out";
            command->args[0] = "help.out";
        }

        if (COMMAND_IS_("history")){
            command->name = "./build-in/history/history.out";
            command->args[0] = "history.out";
        }

        int child_pid = 0;
        int status = 0;
        if ((child_pid = fork())){
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
                
            int return_val = execvp(command->name, command->args);              

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
            
            if (return_val == -1){
                printc(BOLD_RED, "Error executing command '%s'\n", command->name);
                exit(1);
            }
            exit(0);
        }
    }    
}

int execute_io_command(command_t* cmd){
    if (cmd->out_symbol){
        if (STR_EQ(cmd->out_symbol, ">")){
            FILE* fd = fopen(cmd->out_arg, "w+");
            if (fd->_fileno < 2){
                printc(RED, "Error creating file %s\n", cmd->out_arg);
            }
            else{
                cmd->out = fd;
            }
        }
        else if (STR_EQ(cmd->out_symbol, ">>")){
            FILE* fd = fopen(cmd->out_arg, "a+");
            if (fd->_fileno < 2){
                printc(RED, "Error opening file %s\n", cmd->out_arg);
            }
            else{
                cmd->out = fd;
            }
        }
    }
    if (cmd->in_arg){
        FILE* fd = fopen(cmd->in_arg, "r");
        if (fd->_fileno < 2){
            printc(RED, "Error opening file %s\n", cmd->in_arg);
        }
        else{
            cmd->in = fd;
        }
    }
    return execute_command(cmd);
}

int execute_piped_command(piped_command_t* piped_cmd){
    for (int i = 1; i <= piped_cmd->pipes_count; i++){
        int p[2];
        pipe(p);
        piped_cmd->commands[i-1]->p_out = p[1];
        piped_cmd->commands[i]->p_in = p[0];
    }
    for (int i = 0; i <= piped_cmd->pipes_count; i++){
        if (!execute_io_command(piped_cmd->commands[i])){
            return 0;
        }
    }
    return 1;
}

int execute_logic_command(logic_command_t* logic_cmd){
    int result = execute_piped_command(logic_cmd->piped_command);
    if (logic_cmd->operator){
        if (STR_EQ(logic_cmd->operator, "&&") && result){
            return execute_logic_command((logic_command_t*)logic_cmd->logic_command);
        }
        else if (STR_EQ(logic_cmd->operator, "||") && !result){
            return execute_logic_command((logic_command_t*)logic_cmd->logic_command);            
        }
    }
    return result;
}

int execute_comand_line(command_line_t* cmd_line){
    int result = 0;
    for (int i = 0; i < cmd_line->commands_count; i++){
        result = execute_logic_command(cmd_line->logic_commands[i]);
    }   
    return result; 
}

int execute_line(line_t* line){
    int if_result = 0;
    if (line->if_command_line){
        if_result = execute_line((line_t*)line->if_command_line);
        if (if_result && line->then_command_line){
            return execute_line((line_t*)line->then_command_line);
        }
        else if (!if_result && line->else_command_line){
            return execute_line((line_t*)line->else_command_line);
        }
    }
    else{
        return execute_comand_line(line->command_line);
    }
    return if_result;    
}

char* separate_pipes(char* line){
    char* answ = (char*)calloc(500, sizeof(char));
    int len = strlen(line);
    int answ_index = 0;
    for (int i = 0; i < len; i++)
    {
        if (i > 0 && i < len - 1 && line[i] == '|' && line[i - 1] != '|' && line[i + 1] != '|'){
            answ[answ_index++] = ' ';
            answ[answ_index++] = '|';
            answ[answ_index++] = ' ';
        }
        else{
            answ[answ_index++] = line[i];            
        }
    }
    answ[answ_index] = 0;
    return answ;
}

int is_digit(char c){
    return c >= '0' && c <= '9';
}

char* replace_again_commands(char* line)
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
            while (hist_line[hp] && hist_line[hp] != '\n'){
                newLine[pos++] = hist_line[hp++];
            }
            newLine[pos++] = ' ';
            i = temp - 1;
        }
        else{
            newLine[pos++] = line[i];
        }
    }
    newLine[pos] = 0;
    return newLine;    
}


void execute_shell_line(char* line){
    if (!history)
        history = init_history_handler();

    char* l1 = separate_pipes(line);
    char* l2 = replace_again_commands(strdup(l1));
    // free(line);
    // free(l1);

    int index = 0;
    line_t* cmd_line = parse_line(l2, &index, -1);

    char* str = line_str(cmd_line);
    add_line(str, history);

    execute_line(cmd_line);

}