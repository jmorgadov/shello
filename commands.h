#ifndef commandExe_H
#define commandExe_H

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
    command_t left;
    command_t right;    
}command_t;

void execute(command_t* command);

#endif
