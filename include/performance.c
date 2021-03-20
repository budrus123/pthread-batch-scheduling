#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "performance.h"
#include "job.h"

extern struct job completed_jobs[JOB_BUF_SIZE * 10];

/*
* Function to print the performance measures.
*/

void print_performance_measures() {
	if (completed_job_index == 0) {
		return;
	}
	printf("\n--------------------------------------------------------\n");
	printf("\t\tPerformance info below\n");
	printf("--------------------------------------------------------\n");
	int total_number_of_jobs = performance_metrics.total_number_of_jobs;
	double average_ta = performance_metrics.total_turnaround_time / total_number_of_jobs;
	double average_cpu = performance_metrics.total_cpu_time / total_number_of_jobs;
	double average_wait = performance_metrics.total_waiting_time / total_number_of_jobs;
	double throughput = performance_metrics.throughput;
	printf("Total number of jobs submitted:\t%5.2f\n", (double)total_number_of_jobs);
	printf("Average turnaround time:\t%5.2f\n", average_ta);
	printf("Average CPU time:\t\t%5.2f\n", average_cpu);
	printf("Average waiting time:\t\t%5.2f\n", average_wait);
	printf("Throughput:\t\t\t%5.2f\n", throughput);

}

/*
* Function to compute the performance measures
*/

void compute_performance_measures() {
	pthread_mutex_lock(&completed_job_queue_lock);
	if (completed_job_index == 0) {
		printf("No jobs have completed, no info to display.\n");
		return;
	}

	int total_number_of_jobs = completed_job_index;
	performance_metrics.total_number_of_jobs = total_number_of_jobs;
	double sum_turnaround, sum_cpu_time, sum_wait_time;
	int i = 0;
	while (i < completed_job_index) {
		performance_metrics.total_turnaround_time += completed_jobs[i].turnaround_time;
		performance_metrics.total_cpu_time += completed_jobs[i].cpu_time;
		performance_metrics.total_waiting_time += completed_jobs[i].wait_time;
		i++;
	}
	time_t end_time = performance_metrics.program_end_time;
	time_t start_time = performance_metrics.program_start_time;

	performance_metrics.throughput = total_number_of_jobs / (difftime(end_time, start_time));
	pthread_mutex_unlock(&completed_job_queue_lock);

}