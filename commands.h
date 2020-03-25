#ifndef commandExe_H
#define commandExe_H

#define SET_IN(comm, n) comm->in = n
#define SET_OUT(comm, n) comm->out = n

typedef enum{
    system,
    shell,
    file,
    special
}CommandType;

typedef struct command{
    char* name;
    char** args;    
    CommandType type;
}command_t;

void execute(command_t* command);

command_t* init_command(char* name, char** args, CommandType type);

#endif
