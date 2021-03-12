#include <stdlib.h>
#include <stdio.h>

typedef enum { 
	FCFS, 
	SJF, 
	PRIORITY, 
	NONE
} Policy;

Policy policy = NONE;
int policy_change = 0;

