#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/file_handle.h"
#include "../include/proc.h"


static void
usage(void)
{
    extern char *__progname;
	fprintf(stderr, "Usage: %s [-r] [-f output_filename]...\n",__progname);
	exit(1);
}

/*
TO DO
- Parse command line options better as they are order dependant at the moment
    - This will then allow for further command line arguments such as hiding kernel processes etc..
    - Add option allowing the user to filter the fields returned
    - Add the option to highlight in RED any potential suspicious processes (from a pre-defined list)
*/
int main(int argc, char **argv)
{
    int mode = 0;

    if(argc < 2) {
        usage();
    }

    if(strncmp("-f", argv[1], 2) == 0) {
        if(argc != 3)
            usage();
        mode = 1;
    }else if(strncmp("-r", argv[1], 2) == 0) {
        mode = 0;
    } else{
        usage();
    }

    //Before continuing if we are in file mode we need to validate that we can write to the file
    if(mode == 1) {
        if(check_file_write(argv[2]) == 0) {
            perror("Error can't write to the file name/path specified");
            exit(-1);
        }
    }

    //Drop into proc.c where the rest of this program will be handled
    proc_parse(mode, argv);

    return 0;
}
