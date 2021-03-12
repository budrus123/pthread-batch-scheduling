#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "job.h"

void print_job_info(struct job new_job){
	printf("%s\t",new_job.job_name);
	printf("%4.2f\t\t",new_job.cpu_time);
	printf("%d\t",new_job.priority);
	char* arrive_time = ctime(&new_job.arrival_time);
	arrive_time[strlen(arrive_time)-5] = '\0';
	printf("%s",arrive_time+11);
}

void execute_job_process(struct job executing_job) {
	float cpu_time = executing_job.cpu_time;
	char float_in_string[10];
	gcvt(cpu_time, 4, float_in_string);
	char *my_args[3];  
  	my_args[0] = "./job_process";
  	my_args[1] = float_in_string;
  	my_args[2] = NULL;
  	execv("./job_process", my_args);
}