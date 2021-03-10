CC=gcc
CFLAGS=-g -lm

all: main job_process

main : main.o command_line.o job_queue.o
	$(CC) -pthread -o main command_line.o job_queue.o main.o

job_process : job_process.o
	$(CC) -o job_process job_process.c

.PHONY : clean

clean : 
	rm *.o $(objects) main

