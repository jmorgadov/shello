#ifndef commandExe_H
#define commandExe_H

#include <stdio.h>
#include "./parser/ast_structs.h"
void set_init_path(char* path);
void execute_shell_line(char* line);
#endif
