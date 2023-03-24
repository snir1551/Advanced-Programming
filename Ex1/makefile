CC=gcc 
FLAGS= -Wall -g


all: slast



slast:	slast.o
	$(CC) slast.c -o slast
	


slast.o: slast.c
	$(CC) $(FLAGS) -c slast.c



.PHONY: clean

clean: 
	rm -f *.o slast

