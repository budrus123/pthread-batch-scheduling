#include <stdlib.h>
#include <stdio.h>


struct Job {
	int id;
	char *job_name;
	int arrival_time;
	int priority;
	int cpu_time;
	int finish_time;
	// int wait_time; //Debate whether to add it
	// int response_time; //Debate whether to add it

};