#include <stdlib.h>
#include <stdio.h>

struct job {
	int id;
	char job_name [20];
	time_t arrival_time;
	int priority;
	double cpu_time;
	time_t finish_time;
	double turnaround_time;
	double wait_time; //Debate whether to add it
	int response_time; //Debate whether to add it

};

void execute_job_process(struct job executing_job);

void print_job_info(struct job new_job);