CC=gcc
CFLAGS=-g -lm -Iinclude
HEADERS = include/
BENCHMARK = benchmark/

all: main batch_job sleep_job

main : main.o $(HEADERS)command_line.o $(HEADERS)job.o $(HEADERS)policy.o $(HEADERS)performance.o
	$(CC) -pthread -o main $(HEADERS)command_line.o $(HEADERS)job.o $(HEADERS)policy.o $(HEADERS)performance.o main.o

batch_job : $(BENCHMARK)batch_job.o
	$(CC) -o batch_job $(BENCHMARK)batch_job.c

sleep_job : $(BENCHMARK)sleep_job.o
	$(CC) -o sleep_job $(BENCHMARK)sleep_job.c

.PHONY : clean

clean : 
	rm *.o $(objects) main batch_job sleep_job

