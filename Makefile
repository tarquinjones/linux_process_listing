CC = gcc

CFLAGS = -Wall

INCLUDES = -I./include

SRCS = src/main.c src/proc.c src/string_format.c src/file_handle.c

OBJS = $(SRCS:.c=.o)

MAIN = process_listing

.PHONY: depend clean

all:    $(MAIN)
	@echo Compiled to process_listing 

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) src/*.o *~ $(MAIN)
