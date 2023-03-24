CC=gcc 
FLAGS= -Wall -g


all: stree



stree:	stree.o
	$(CC) stree.c -o stree
	


stree.o: stree.c
	$(CC) $(FLAGS) -c stree.c



.PHONY: clean

clean: 
	rm -f *.o stree

