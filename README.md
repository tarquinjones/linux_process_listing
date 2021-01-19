# Linux process listing

*For those times when you don't trust ps or have it?*

It's pretty simple at the moment compile and run it! I'll add a Makefile soon so simplify it.

## Tested on
* Debian
* Ubuntu 18.04+

## Usage 

There are two modes available when using the tool.

### Raw Mode
This will display all the information to stdout. 
Kernel process are highlighted in yellow.
```
./process_listing -r
```

### File mode
This mode will output each process into a tab delimeted file.
```
./process_listing -f /home/user/processes.csv
```

### Compile using make
```
git clone https://this_repo && make
```
