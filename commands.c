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
#include "pids_handler.h"
#include "parser/ast_structs.h"
#include "parser/parser.h"
#include <errno.h>

typedef int bool;

#define STR_EQ(s1,s2) strcmp(s1,s2) == 0
#define COMMAND_IS_(s) STR_EQ(command->name, s)

#define MAX_COMMAND_IN_LINE 10

char* initial_path = NULL;

history_h* history = NULL;
pids_h* pids = NULL;

int default_p_in = 0;
int default_p_out = 1;
FILE* default_f_in = NULL;
FILE* default_f_out = NULL;

void set_init_path(char* path){
    initial_path = path;    
}

int execute_if_command(if_command_t* if_cmd, int* error);

int execute_command(command_t* command, int* error){
    if (command->if_command){
        int last_dp_in = default_p_in;
        int last_dp_out = default_p_out;
        FILE* last_df_in = default_f_in;
        FILE* last_df_out = default_f_out;

        if (command->in){
            default_f_in = command->in;
        }
        if (command->out){
            default_f_out = command->out;
        }
        if (command->p_in != 0){
            default_p_in = command->p_in;
        }
        if (command->p_out != 1){
            default_p_out = command->p_out;
        }

        int ret_val = execute_if_command((if_command_t*)command->if_command, error);

        if (default_p_in != 0 && default_p_in != last_dp_in){
            close(default_p_in);
        }
        if (default_p_out != 1 && default_p_out != last_dp_out){
            close(default_p_out);
        }

        if (default_f_in && default_f_in->_fileno != 0 && default_f_in != last_df_in){
            fclose(default_f_in);
        }
        if (default_f_out && default_f_out->_fileno != 1 && default_f_out != last_df_out){
            fclose(default_f_out);
        }

        default_p_in = last_dp_in;
        default_p_out = last_dp_out;
        default_f_in = last_df_in;
        default_f_out = last_df_out;
        return ret_val;
    }

    if (!command->args_count){
        return 1;
    }

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
            char* help_path = (char*)calloc(500, sizeof(char));
            strcpy(help_path,initial_path);
            strcat(help_path, "/build-in/help/help.out");
            command->name = help_path;
            command->args[0] = help_path;
            if (command->args[1]){
                char* comm = strdup(command->args[1]);
                command->args[1] = strdup(initial_path);
                command->args[2] = comm;
                command->args[3] = NULL;
            }
            else{
                command->args[1] = strdup(initial_path);
                command->args[2] = NULL;
            }
        }

        if (COMMAND_IS_("history")){
            char* hist_path = (char*)calloc(500, sizeof(char));
            strcpy(hist_path,initial_path);
            strcat(hist_path, "/build-in/history/history.out");
            command->name = hist_path;
            command->args[0] = hist_path;
        }

        if (!command->in){
            command->in = default_f_in;
        }
        if (!command->out){
            command->out = default_f_out;
        }

        if (command->p_in == 0 && !command->in){
            command->p_in = default_p_in;
        }
        if (command->p_out == 1 && !command->out){
            command->p_out = default_p_out;
        }

        int child_pid = 0;
        int status = 0;
        if ((child_pid = fork())){
            add_pid(pids, child_pid, command);  
            FILE* out = command->out;
            FILE* in = command->in;
            if (out && out->_fileno != 0 && out->_fileno != 1 && out != default_f_out){
                fclose(out);
            }        
            if (in && in->_fileno != 0 && in->_fileno != 1 && in != default_f_in){
                fclose(in);                
            } 
            if (command->p_out != default_p_out && command->p_out != 1)
                close(command->p_out);
            if (command->p_in != default_p_in && command->p_in != 0)
                close(command->p_in);            
            return status == 0;
        }
        else{

            int i = default_p_in;
            int o = default_p_out;
            if (command->in)
                i = dup2(command->in->_fileno, 0);
            else if (command->p_in != 0)
                i = dup2(command->p_in, 0);

            if (command->out)
                o = dup2(command->out->_fileno, 1);
            else if (command->p_out != 1)
                o = dup2(command->p_out, 1);

            if (i != 0)
                close(i);
            if (o != 1)
                close(o);
                
            execvp(command->name, command->args);            
            int es = errno;
            FILE* out = command->out;
            FILE* in = command->in;
            if (out && out->_fileno != 0 && out->_fileno != 1){
                fclose(out);
            }        
            if (in && in->_fileno != 0 && in->_fileno != 1){
                fclose(in);                
            }  

            if (command->p_out != 1)
                close(command->p_out);
            if (command->p_in != 0)
                close(command->p_in);   
                
            if (es)
                perror("shello");  

            exit(es);
        }
    }    
}

int execute_io_command(command_t* cmd, int* error){
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
    return execute_command(cmd, error);
}

int execute_piped_command(piped_command_t* piped_cmd, int* error){
    int commands_count = 0;
    piped_command_t* temp = piped_cmd;

    while (temp){
        commands_count++;
        temp = (piped_command_t*)temp->piped_command;
    }

    command_t** commands = (command_t**)calloc(commands_count, sizeof(command_t*));

    temp = piped_cmd;
    for (int i = 0; i < commands_count; i++)
    {
        commands[i] = temp->command;
        temp = (piped_command_t*)temp->piped_command;            
    }

    for (int i = 1; i < commands_count; i++)
    {
        int p[2];
        pipe(p);
        commands[i - 1]->p_out = p[1];
        commands[i]->p_in = p[0];
    }

    for (int i = 0; i < commands_count; i++)
    {
        execute_io_command(commands[i], error);            
    }

    int status = 0;
    for (int i = 0; i < pids->pids_count; i++)
    {          
        waitpid(pids->pids[i], &status, 0);
        *error = status;
    }       

    if (*error) return 0;
    return status == 0;
}

int execute_logic_command(logic_command_t* logic_cmd, int* error){
    int result = execute_piped_command(logic_cmd->piped_command, error);
    if (*error) return 0;
    if (logic_cmd->operator){
        if (STR_EQ(logic_cmd->operator, "&&") && result){
            return execute_logic_command((logic_command_t*)logic_cmd->logic_command, error);
        }
        else if (STR_EQ(logic_cmd->operator, "||") && !result){
            return execute_logic_command((logic_command_t*)logic_cmd->logic_command, error);            
        }
    }
    return result;
}

int execute_comand_line(command_line_t* cmd_line, int* error){
    int result = execute_logic_command(cmd_line->logic_command, error);
    if (*error) return 0;
    if (cmd_line->command_line){
        result = execute_comand_line(cmd_line->command_line, error);
    }    
    return result; 
}

int execute_if_command(if_command_t* if_cmd, int* error){
    int if_result = 0;
    if_result = execute_comand_line(if_cmd->if_command_line, error);
    if (*error) return 0;
    if (if_result && if_cmd->then_command_line){
        return execute_comand_line(if_cmd->then_command_line, error);
    }
    else if (!if_result && if_cmd->else_command_line){
        return execute_comand_line(if_cmd->else_command_line, error);
    }
    return if_result;    
}

int is_digit(char c){
    return c >= '0' && c <= '9';
}

char* replace_again_commands(char* line, int* error)
{
    int len = strlen(line);
    char* newLine = (char*)malloc(sizeof(char)*500);
    char* number = (char*)malloc(sizeof(char)*10);
    int pos = 0;
    int on_string = 0;
    for (int i = 0; i < len; i++)
    {
        if (line[i] == '\"'){
            if (on_string) 
                on_string = 0;
            else
                on_string = 1;
            continue;
        }
        if (!on_string && starts_with(line + i, "again") && (i == 0 || (i > 0 && line[i - 1] != '\\'))){
            int temp = i;
            temp += 5;
            while (line[temp] == ' ')
                temp++;
            int np = 0;
            while (temp < len && is_digit(line[temp])){
                number[np++] = line[temp++];
            }
            if (np == 0){
                *error = 1;
                printc(BOLD_RED, "No number after \'again\' command\n");
                printc(BOLD_YELLOW, "NOTE: If you want to escape \'again\' command type \'\\again\'\n");
                return NULL;
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

char* comment[1] = {"#"};

void execute_shell_line(char* line){
    if (!history)
        history = init_history_handler();

    if (!pids)
        pids = init_pids_handler();

    reset_pidsh(pids);

    int error = 0;

    int vi = 0;
    int occ = first_occurrense(line, comment, 1, 0, &vi);
    char* l1 = strdup(line);
    if (occ != -1){
        strncpy(l1, line, occ);
        l1[occ] = 0;
    }
    char* l2 = replace_again_commands(strdup(l1), &error);

    if (error){
        return;
    }

    char* line_repr = NULL;
    command_line_t* cmd_line = parse_line(l2, &line_repr, &error);

    if (error){
        return;
    }

    add_line(line_repr, history);
    execute_comand_line(cmd_line, &error);
}