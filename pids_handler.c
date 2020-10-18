#include "pids_handler.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

pids_h* init_pids_handler(){
    pids_h* ph = (pids_h*)malloc(sizeof(pids_h));
    ph->pids_count = 0;
    ph->pids = (int*)calloc(1000, sizeof(int));
    ph->pids_signal_status = (int*)calloc(1000, sizeof(int));
    ph->commands = (command_t**)calloc(1000, sizeof(command_t*));
    return ph;
}

void reset_pidsh(pids_h* handler){
    for (int i = 0; i < handler->pids_count + 1; i++)
    {
        handler->pids[i] = 0;
        handler->pids_signal_status[i] = 0;
    }
    handler->pids_count = 0;
}

void add_pid(pids_h* handler, int pid, command_t* command){
    handler->pids[handler->pids_count] = pid;
    handler->commands[handler->pids_count] = command;
    handler->pids_signal_status[handler->pids_count] = 0;
    handler->pids_count += 1;
}