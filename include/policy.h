#include <stdlib.h>
#include <stdio.h>

typedef enum { 
	FCFS, 
	SJF, 
	PRIORITY, 
	NONE
} Policy;

extern Policy policy;
extern int policy_change;
extern pthread_mutex_t job_queue_lock; 

int sjf();
int fcfs();
int priority();
void print_policy();


