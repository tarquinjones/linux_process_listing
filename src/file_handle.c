#include <stdio.h>

int check_file_write(const char *fpath) {

    FILE *fp = fopen(fpath, "w");

    if(fp == NULL)
        return 0;

    //Now we know we can write we write the csv headers
    fprintf(fp, "PID\tPPID\tName\tCommand Line\tState\tPGRP\tSESSID\tRUID\tEUID\tSSUID\tRGID\tEGID\tSSGID\tFSGID\tTTY_NR\tTHREADS\tVMSIZE\tStart Time");
    fclose(fp);
    return 1;
}
