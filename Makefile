CC=gcc
CFLAGS=-g -lm

all: main batch_job

main : main.o command_line.o job.o policy.o
	$(CC) -pthread -o main command_line.o job.o policy.o main.o

batch_job : batch_job.o
	$(CC) -o batch_job batch_job.c

.PHONY : clean

clean : 
	rm *.o $(objects) main

