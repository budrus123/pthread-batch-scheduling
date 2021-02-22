#include <stdlib.h>
#include <stdio.h>


struct job {
	int id;
	char job_name [20];
	int arrival_time;
	int priority;
	int cpu_time;
	int finish_time;
	// int wait_time; //Debate whether to add it
	// int response_time; //Debate whether to add it

};