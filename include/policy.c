#include "policy.h"
#include "job.h"

void change_queue_to_fcfs(struct job job[], int count);
void change_queue_to_sjf(struct job job[], int count);
void change_queue_to_priority(struct job job[], int count);

/*
* Function to print the current set policy.
*/
void print_policy() {
	switch(policy) {
		case FCFS:
		printf("FCFS");
		break;
		case PRIORITY:
		printf("PRIORITY");
		break;
		case SJF:
		printf("SJF");
		break;
	}
}


/*
* Function to change the scheduling algorithm to
* FCFS and set a flag that the policy has been
* changed, so the schedular can reschedle jobs
* in the queue.
*/

int fcfs(){
	pthread_mutex_lock(&job_queue_lock);
	int count_queue = get_count_elements_in_queue();
	printf("Scheduling policy is switched to FCFS.");
	if (count_queue > 0) {
		printf(" All the %d waiting jobs have been rescheduled.\n", count_queue);
	} else {
		printf(" No jobs in pending rescheduling.\n");
	}
	policy_change = 1;
	policy = FCFS;
	pthread_mutex_unlock(&job_queue_lock);

}

/*
* Function to change the scheduling algorithm to
* SJF and set a flag that the policy has been
* changed, so the schedular can reschedle jobs
* in the queue.
*/

int sjf(){
	pthread_mutex_lock(&job_queue_lock);
	int count_queue = get_count_elements_in_queue();
	printf("Scheduling policy is switched to SJF.");

	if (count_queue > 0) {
		printf(" All the %d waiting jobs have been rescheduled.\n", count_queue);
	} else {
		printf(" No jobs in pending rescheduling.\n");
	}
		policy_change = 1;
	policy = SJF;
	pthread_mutex_unlock(&job_queue_lock);
	
}

/*
* Function to change the scheduling algorithm to
* priority and set a flag that the policy has been
* changed, so the schedular can reschedle jobs
* in the queue.
*/

int priority(){
	pthread_mutex_lock(&job_queue_lock);
	int count_queue = get_count_elements_in_queue();
	printf("Scheduling policy is switched to Priority.");
	if (count_queue > 0) {
		printf(" All the %d waiting jobs have been rescheduled.\n", count_queue);

	} else {
		printf(" No jobs in pending rescheduling.\n");
	}

	policy_change = 1;
	policy = PRIORITY;
	pthread_mutex_unlock(&job_queue_lock);
}


/*
* Function to update the policy of the elements 
* in the queue.
*/
void update_policy(Policy policy) {
	int elements_in_queue = get_count_elements_in_queue();
	struct job temp_jobs [elements_in_queue];
	int temp_tail = tail;
	int temp_head = head;

	int i = 0;
	while (temp_tail < temp_head) {
		temp_jobs[i] = job_queue[temp_tail];
		temp_tail = (temp_tail + 1) % JOB_BUF_SIZE;
		i++;
	}

	switch (policy){
		case FCFS:
		change_queue_to_fcfs(temp_jobs, elements_in_queue);
		break;

		case SJF:
		change_queue_to_sjf(temp_jobs, elements_in_queue);
		break;

		case PRIORITY:
		change_queue_to_priority(temp_jobs, elements_in_queue);
		break;
	}
}


/*
* Function that takes an array of jobs, and their count
* and sorts them depending on arrival time.
*/
void change_queue_to_fcfs(struct job temp_jobs[], int count) {

	int i, j;
	for (i = 0; i < count -1 ; i++) {
		for (j=0; j < count-i-1; j++) {
			if (temp_jobs[j].arrival_time > temp_jobs[j+1].arrival_time) {
				//swapping
				struct job temp = temp_jobs[j];
				temp_jobs[j] = temp_jobs[j+1];
				temp_jobs[j+1] = temp;
			}
		}
	}

	int temp_tail = tail;
	int temp_head = head;

	i = 0;
	while (temp_tail < temp_head) {
		job_queue[temp_tail] = temp_jobs[i];
		temp_tail = (temp_tail + 1) % JOB_BUF_SIZE;
		i++;
	}
}

/*
* Function that takes an array of jobs, and their count
* and sorts them depending on burst time (SJF).
*/
void change_queue_to_sjf(struct job temp_jobs[], int count) {
	
	int i, j;
	for (i = 0; i < count -1 ; i++) {
		for (j=0; j < count-i-1; j++) {
			if (temp_jobs[j].cpu_time > temp_jobs[j+1].cpu_time) {
				//swapping
				struct job temp = temp_jobs[j];
				temp_jobs[j] = temp_jobs[j+1];
				temp_jobs[j+1] = temp;
			}
		}
	}

	int temp_tail = tail;
	int temp_head = head;

	i = 0;
	while (temp_tail < temp_head) {
		job_queue[temp_tail] = temp_jobs[i];
		temp_tail = (temp_tail + 1) % JOB_BUF_SIZE;
		i++;
	}


}

/*
* Function that takes an array of jobs, and their count
* and sorts them depending on priority (higher priority).
*/
void change_queue_to_priority(struct job temp_jobs[], int count) {
	int i, j;
	for (i = 0; i < count -1 ; i++) {
		for (j=0; j < count-i-1; j++) {
			if (temp_jobs[j].priority < temp_jobs[j+1].priority) {
				//swapping
				struct job temp = temp_jobs[j];
				temp_jobs[j] = temp_jobs[j+1];
				temp_jobs[j+1] = temp;
			}
		}
	}

	int temp_tail = tail;
	int temp_head = head;

	i = 0;
	while (temp_tail < temp_head) {
		job_queue[temp_tail] = temp_jobs[i];
		temp_tail = (temp_tail + 1) % JOB_BUF_SIZE;
		i++;
	}
}
