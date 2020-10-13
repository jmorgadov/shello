#ifndef PARSER_H
#define PARSER_H

line_t* parse_line(char *line, int* line_index, int last_cond);
char* line_str(line_t* cmd);
#endif