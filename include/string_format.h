#include <regex.h>

#ifndef STRING_FORMAT_H
#define STRING_FORMAT_H

#include "proc.h"

#endif

#define STAT_FMTSTR "%d %c %d %d %d %*d %d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %ld %*ld %llu %lu %*ld"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

char *format_path(char *core, char *new_element);
char *get_raw_value(char *base, size_t start, int del_spaces);
char *trim_before(char *line);
char *trim_nl(char *line);
int remove_statpname(char **line);
void reg_compare(char *line, char *pattern, int *reg_matches);
void convert_starttime(long btime, long sc_clk_tck, pidInfo *procDetails);
void output_pid(char **argv, pidInfo *procDetails);
void print_output(pidInfo *procDetails);
char *get_next_line(FILE *fp);
