#include "policy.h"

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
	printf("Scheduling policy is switched to FCFS." 
		" All the %d waiting jobs have been rescheduled.\n", count_queue);	
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
	printf("Scheduling policy is switched to SJF." 
		" All the %d waiting jobs have been rescheduled.\n", count_queue);	
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
	printf("Scheduling policy is switched to Priority." 
		" All the %d waiting jobs have been rescheduled.\n", count_queue);	
	policy_change = 1;
	policy = PRIORITY;
	pthread_mutex_unlock(&job_queue_lock);
}

