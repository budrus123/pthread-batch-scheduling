#include <stdlib.h>
#include <stdio.h>

#define JOB_BUF_SIZE 50


struct job {
	int id;
	char job_name [20];
	time_t arrival_time;
	time_t start_time;
	int priority;
	double cpu_time;
	time_t finish_time;
	double turnaround_time;
	double wait_time; //Debate whether to add it
	int response_time; //Debate whether to add it

};

extern int head;
extern int tail;
extern int completed_job_index;
extern int currently_executing;
char benchmark_name [30];

extern struct job job_queue[JOB_BUF_SIZE];
extern struct job completed_jobs[JOB_BUF_SIZE * 10];
extern struct job running_job;

void print_job_info(struct job new_job);
void execute_job_process(struct job executing_job);
void fill_job_details(struct job* completed_job);

int queue_full();
int get_next_position();
struct job dequeue();
struct job enqueue(struct job new_job);
int get_count_elements_in_queue();
int queue_empty();

void list_all_jobs();
int get_expected_wait_time();

