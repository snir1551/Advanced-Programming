CC=gcc 
FLAGS= -Wall -g


all: shell



shell:	shell.o
	$(CC) shell.c -o shell
	


shell.o: shell.c
	$(CC) $(FLAGS) -c shell.c



.PHONY: clean

clean: 
	rm -f *.o shell

