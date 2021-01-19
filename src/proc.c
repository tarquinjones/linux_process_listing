#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <linux/kdev_t.h>
#include <unistd.h>
#include "../include/proc.h"
#include "../include/string_format.h"

void proc_parse(int mode, char **argv) {

    DIR *procdir;
    char *pid_folder = NULL;
    struct dirent *procdir_ent;

    procdir = opendir(PROC_DIR);

    if(procdir == NULL) {
        perror("Error on proc directory");
        exit(-1);
    }

    //Before processing pids get boot time and clock ticks
    long sc_clk_tck = sysconf(_SC_CLK_TCK);
    long btime = get_boot_time();
    if(btime == -1){
        perror("Error getting boot time...\n");
        exit(-1);
    }

    //Loop through all proc entries
    while( (procdir_ent = readdir(procdir)) != NULL) {
        //Check to see that the entry is a directory and a number
        if( procdir_ent->d_type == DT_DIR && atoi(procdir_ent->d_name) > 0 ) {
            //Form the directory for further parsing +2 is for the NULL byte and "/"
            pid_folder = malloc(strlen(PROC_DIR)+strlen(procdir_ent->d_name) + 2);
            if(pid_folder != NULL) {
                memcpy(pid_folder, PROC_DIR, strlen(PROC_DIR));
                memcpy(pid_folder+strlen(PROC_DIR), "/", 1);
                memcpy(pid_folder+strlen(PROC_DIR)+1, procdir_ent->d_name, strlen(procdir_ent->d_name));
                pid_folder[strlen(PROC_DIR)+strlen(procdir_ent->d_name)+1] = '\0';

                //Struct for process information to be filled
                pidInfo *procDetails = (pidInfo *)malloc(sizeof(pidInfo));
                if(processPid(btime, sc_clk_tck, pid_folder, procDetails) != -1) {
                    output_pid(argv, procDetails);
                }
                free_struct(procDetails);
                free(pid_folder);
            }
        }
    }

}

long get_boot_time() {
    FILE *fp;
    char *line;
    long btime;

    fp = fopen("/proc/stat", "r");
    if(fp == NULL)
        return -1;
    while((line = get_next_line(fp)) != NULL) {
        if(memcmp(line, "btime", 5) == 0) {
            sscanf(line, "%*s %ld", &btime);
        }
        free(line);
    }
    return btime;
}

/*
All the elements where we've malloced memory for we free
*/
void free_struct(pidInfo *procDetails) {

        if(procDetails->cmdline) {
            free(procDetails->cmdline);
        }
        if(procDetails->pname) {
            free(procDetails->pname);
        }
        free(procDetails);
}

/*
This function handles spawing off to other functions to gather the information required for each process
- Most of the information is taken from the status file as it's simple to read from and the raw values can be used
- A few fields such as what devices the process is using and process start time are retrieved
*/
int processPid(long btime, long sc_clk_tck, char *pid_folder, pidInfo *procDetails) {

    int retcode = -1;

    if(get_cmdline(pid_folder, procDetails) == -1) {
        return retcode;
    }

    if(get_procstatus(pid_folder, procDetails) == -1) {
        return retcode;
    }

    if(get_procstat(pid_folder, procDetails) == -1) {
        return retcode;
    }

    convert_starttime(btime, sc_clk_tck, procDetails);

    retcode = 1;
    return retcode;
}

void parse_Statusline(char *line, pidInfo *procDetails) {
    if(memcmp(line, "Name", 4) == 0) {
        procDetails->pname = trim_nl(trim_before(get_raw_value(line, 5, 0)));
    } else if(memcmp(line, "Uid", 3) == 0) {
        sscanf(get_raw_value(line, 4, 0), "%d %d %d %d", &procDetails->ruid, &procDetails->euid, &procDetails->ssuid, &procDetails->fsuid);
    } else if(memcmp(line, "Gid", 3) == 0) {
        sscanf(get_raw_value(line, 4, 0), "%d %d %d %d", &procDetails->rgid, &procDetails->egid, &procDetails->ssgid, &procDetails->fsgid);
    }

}

/*
Before we use the stat line we need to format it to remove the process name from the line.
- Reasoning is that it will break the format specifier due to the potential for spaces in the pname.
*/
int get_procstat(char *pid_folder, pidInfo *procDetails) {
    FILE *statfp;
    int ret_code = -1;
    char *line = NULL;
    char *stat_path = format_path(pid_folder, "stat");

    statfp = fopen(stat_path, "r");
    if (statfp == NULL) {
        perror("Error: ");
        return ret_code;
    }

    line = get_next_line(statfp);

    if(remove_statpname(&line) != -1) {
        sscanf(line, STAT_FMTSTR, &procDetails->pid, &procDetails->state, &procDetails->ppid, &procDetails->pgrp, &procDetails->sessid, &procDetails->tty_nr, &procDetails->nthreads, &procDetails->starttime ,&procDetails->vmsize);
        ret_code = 1;
    }
    fclose(statfp);
    free(line);
    return ret_code;

}

int get_procstatus(char *pid_folder, pidInfo *procDetails) {

    FILE *statusFP;
    char *line = NULL;

    char *status_pth = format_path(pid_folder, "status");
    statusFP = fopen(status_pth, "r");
    if(statusFP == NULL) {
        perror("Error: ");
        return -1;
    }

    while((line = get_next_line(statusFP)) != NULL) {
        parse_Statusline(line, procDetails);
        free(line);
    }

    fclose(statusFP);
    return 1;
}

int get_cmdline(char *pid_folder, pidInfo *procDetails) {

    char *cmdline = NULL;
    char *cmdline_pth = NULL;
    FILE *cmdline_fp;

    cmdline_pth = format_path(pid_folder, "cmdline");

    cmdline_fp = fopen(cmdline_pth, "r");
    if(cmdline_fp == NULL) {
        perror("Error: ");
        return -1;
    }

    cmdline = get_next_line(cmdline_fp);

    if(cmdline != NULL) {
        procDetails->cmdline = strdup(cmdline);
    }else {
        procDetails->cmdline = (char *)malloc(2);
        procDetails->cmdline[0] = '-';
        procDetails->cmdline[1] = '\0';
    }

    free(cmdline);
    fclose(cmdline_fp);
    free(cmdline_pth);
    return 0;
}
