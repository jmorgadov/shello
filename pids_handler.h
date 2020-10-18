#ifndef PIDH_H
#define PIDH_H

#include "./parser/ast_structs.h"

typedef struct pids_handler{
    int* pids;
    int* pids_signal_status;
    command_t** commands;
    int pids_count;
}pids_h;

pids_h* init_pids_handler();
void reset_pidsh(pids_h* handler);
void add_pid(pids_h* handler, int pid, command_t* command);

#endif