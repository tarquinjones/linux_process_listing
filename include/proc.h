#define PROC_DIR "/proc"
#ifndef PROC_H
#define PROC_H

typedef struct pidInfo{
    char *cmdline;
    char *pname;
    int pid;
    char state;
    int ppid;
    int pgrp;
    int sessid;
    int ruid;
    int euid;
    int ssuid;
    int fsuid;
    int rgid;
    int egid;
    int ssgid;
    int fsgid;
    int tty_nr; //stat position 6
    long nthreads; //stat position 19
    unsigned long vmsize; //stat position 22
    unsigned long long starttime; //stat position 21
} pidInfo;

#endif

void proc_parse(int mode, char **argv);
int processPid(long btime, long sc_clk_tck, char *pid_folder, pidInfo *procDetails);
int get_procstatus(char *pid_folder, pidInfo *procDetails);
int get_cmdline(char *pid_folder, pidInfo *procDetails);
void parse_Statusline(char *line, pidInfo *procDetails);
int get_procstat(char *pid_folder, pidInfo *procDetails);
long get_boot_time();
void free_struct(pidInfo *procDetails);
