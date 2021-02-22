CC=gcc
CFLAGS=-g -lm

main : main.o command_line.o job_queue.o
	$(CC) -o main command_line.o job_queue.o main.o

.PHONY : clean

clean : 
	rm *.o $(objects) main

