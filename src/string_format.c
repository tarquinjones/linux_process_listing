#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "../include/string_format.h"

char *get_next_line(FILE *fp) {
    char *line = NULL;
    int i = 0;
    char c;

    while((c = getc(fp)) != EOF) {
        //Fixes issue with parsing cmdline
        if(c == '\0')
            c = ' ';

        if(i == 0) {
            line = (char *)malloc(sizeof(char *)*2);
            if(line == NULL) {
                perror("Error: ");
                return NULL;
            }
        } else {
            void *tmp = (char *)realloc(line, sizeof(char *)*(i+2));
            if(tmp != NULL) {
                line = tmp;
            } else{
                perror("Error: ");
            }
        }
        line[i+1] = '\0';
        line[i] = c;
        i++;

        if(c == '\n') {
            break;
        }
    }
    return line;
}

char *format_path(char *core, char *new_element) {
    //To form the new element we essentially append it to the end of core with a '/'
    char *new_path = (char *)malloc(sizeof(char *)+strlen(core)+strlen(new_element)+2);
    if(new_path == NULL) {
        perror("Critical Error: ");
        exit(1);
    }
    memcpy(new_path, core, strlen(core));
    memcpy(new_path+strlen(core), "/", 1);
    memcpy(new_path+strlen(core)+1, new_element, strlen(new_element));
    new_path[strlen(core)+strlen(new_element)+1] = '\0';
    return new_path;
}

//Removes the last new line character from a parsed string
char *trim_nl(char *line) {
    char *new_line = (char *)malloc(strlen(line));
    if(new_line == NULL) {
        perror("Critical Error: ");
        exit(1);
    }
    memcpy(new_line, line, strlen(line)-1);
    new_line[strlen(line)-1] = '\0';
    free(line);
    return new_line;
}

/*
Function to trim the leading whitespace
Checks if c is not 1 which is determined once a non white space character is met
*/
char *trim_before(char *line) {
    int c = 0;
    //int i = 0;
    char *new_line = NULL;
    for(int i = 0; c != 1; ++i) {
        if(!isspace(line[i]))
            c++;
    }
    new_line = (char *)malloc(strlen(line)-(c-1));
    if(new_line == NULL) {
        perror("Critical Error: ");
        exit(1);
    }
    memcpy(new_line, line+(c), strlen(line)-(c-1));
    new_line[strlen(line)-(c)] = '\0';
    free(line);
    return new_line;
}

/*
Takes the base and uses the start to return the value that far in from the base
Del spaces with trim spaces from the response
*/
char *get_raw_value(char *base, size_t start, int del_spaces) {
    int i = 0;
    char *new_val = NULL;

    if(!del_spaces){
        new_val = strdup(base+start);
        return new_val;
    } else {
        new_val = (char *)malloc(sizeof(char *)* 2);
    }

    for(; start < strlen(base); start++) {
        if(!isspace(base[start])) {
            if(i == 0) {
                new_val = (char *)malloc(sizeof(char *)* 2);
                if(new_val == NULL) {
                    perror("Critical Error ");
                    exit(1);
                }
            }else {
                void *tmp = (char *)realloc(new_val,sizeof(char *)*(i+2));
                if(tmp != NULL) {
                    new_val = tmp;
                } else {
                    perror("Critical Error ");
                    exit(1);
                }
            }
            new_val[i] = base[start];
            i++;
        }
    }
    new_val[i] = '\0';
    return new_val;
}

/*
Creates a copy of the line to work with
Uses a regex to get byte offsets
Removes this element from the new line before returning.
*/

int remove_statpname(char **line) {

    int reg_matches[2];
    reg_compare(*line, "[ (].*[)]" , reg_matches);

    if(reg_matches[0] == -1) {
        goto exit_error;
    }

    char *new_line = (char *)malloc((strlen(*line)+1)-(reg_matches[0]-reg_matches[1]));

    if(new_line == NULL) {
        goto exit_error;
    }

    memcpy(new_line, *line, reg_matches[0]);
    memcpy(new_line+reg_matches[0], *line+reg_matches[1], strlen(*line)-reg_matches[1]);
    new_line[strlen(*line)-reg_matches[1]] = '\0';
    if(reg_matches[0] != -1) {
        free(*line);
        *line = new_line;
        return 1;
    } else {
        goto exit_error;
    }

exit_error:
    perror("Error whilst processing stat line");
    return -1;
}

void reg_compare(char *line, char *pattern, int *reg_matches) {
    regex_t regex;
    regmatch_t pmatch[2];
    int retv;

    retv = regcomp(&regex, pattern, 0);
    if (retv != 0) {
        perror("Regex compilation error: ");
    }
    retv = regexec(&regex, line, 1, pmatch, 0);
    if(retv != 0) {
        reg_matches[0] = -1;
    } else {
        reg_matches[0] = pmatch[0].rm_so;
        reg_matches[1] = pmatch[0].rm_eo;
    }
}

void convert_starttime(long btime, long sc_clk_tck, pidInfo *procDetails) {
    procDetails->starttime = btime + (procDetails->starttime/sc_clk_tck);
}

void print_output(pidInfo *procDetails) {
    //Highlight kernel processes in yellow
    if(procDetails->ppid == 2) {
        printf(YEL);
    } else {
        printf(RESET);
    }
    fprintf(stdout, "\nProcess Name: %s\n", procDetails->pname);
    fprintf(stdout, "Process Start Time: %llu\n", procDetails->starttime);
    fprintf(stdout, "PID: %d\n", procDetails->pid);
    fprintf(stdout, "PPID: %d\n", procDetails->ppid);
    fprintf(stdout, "Command line: %s\n", procDetails->cmdline);
    fprintf(stdout, "State: %c\n", procDetails->state);
    fprintf(stdout, "PGRP: %d\n", procDetails->pgrp);
    fprintf(stdout, "SESSID: %d\n", procDetails->sessid);
    fprintf(stdout, "RUID: %d\n", procDetails->ruid);
    fprintf(stdout, "EUID: %d\n", procDetails->euid);
    fprintf(stdout, "SSUID: %d\n", procDetails->ssuid);
    fprintf(stdout, "RGID: %d\n", procDetails->rgid);
    fprintf(stdout, "EGID: %d\n", procDetails->egid);
    fprintf(stdout, "SSGID: %d\n", procDetails->ssgid);
    fprintf(stdout, "FSGID: %d\n", procDetails->fsgid);
    fprintf(stdout, "TTY_NR: %d\n", procDetails->tty_nr);
    fprintf(stdout, "Threads: %ld\n", procDetails->nthreads);
    fprintf(stdout, "Virtual Mem size: %lu bytes\n",procDetails->vmsize);
}

/*
Currently two options:
1) Write as csv formatted output to file
2) Write all PID information to stdout
*/
void output_pid(char **argv, pidInfo *procDetails) {

    //file check first
    if(strncmp(argv[1], "-f", 2) == 0) {
        FILE *f = fopen(argv[2], "a");
        if (f == NULL)
        {
            perror("Error: ");
            return;
        }

        fprintf(f, "\n%d\t%d\t%s\t%s\t%c\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%ld\t%lu\t%llu",
            procDetails->pid, procDetails->ppid,procDetails->pname,procDetails->cmdline,procDetails->state,procDetails->pgrp,procDetails->sessid,procDetails->ruid,procDetails->euid,procDetails->ssuid,
            procDetails->rgid,procDetails->egid,procDetails->ssgid,procDetails->fsgid, procDetails->tty_nr, procDetails->nthreads,procDetails->vmsize, procDetails->starttime
        );
        fclose(f);


    } else if(strncmp(argv[1], "-r", 2) == 0) {
        print_output(procDetails);
    }

}
