#ifndef PARSER_H
#define PARSER_H

command_line_t* parse_command_line(char *cmd_line);
char* command_line_str(command_line_t* cmd);
command_line_t* parse_line(char* line, char** line_repr, int* error);
#endif