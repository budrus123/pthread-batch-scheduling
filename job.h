#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct job {
	int id;
	char job_name [20];
	time_t arrival_time;
	int priority;
	float cpu_time;
	time_t finish_time;
	// int wait_time; //Debate whether to add it
	// int response_time; //Debate whether to add it

};