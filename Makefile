CC=gcc
CFLAGS=-g -lm

main : main.o command_line.o
	$(CC) -o main command_line.o main.o

.PHONY : clean

clean : 
	rm *.o $(objects) main

