/*
 * COMP7500/7506
 * Project 3: commandline_parser.c
 *
 * This sample source code demonstrates how to:
 * (1) separate policies from a mechanism
 * (2) parse a commandline using getline() and strtok_r()
 * 
 * The sample code was derived from menu.c in the OS/161 project
 * 
 * Xiao Qin
 * Department of Computer Science and Software Engineering
 * Auburn University
 *
 * Compilation Instruction:
 * gcc commandline_parser.c -o commandline_parser
 * ./commandline_parser
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "command_line.h"
#include "job.h"
// #include "job_queue.h"

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  4 
#define MAXCMDLINE   64 
#define JOB_BUF_SIZE 10

int sjf();
int fcfs();
int priority();

void *sched_function( void *ptr ); 
void *dispatch_function( void *ptr );  

struct Workload_data {
	int number_of_jobs;
	float arrival_rate;
	int min_cpu_time;
	int max_cpu_time;
};

enum Policy { FCFS, SJF, PRIORITY };

struct Perf_info {
	int total_cpu_time;
	int total_waiting_time;
	int total_turnaround_time;
	int total_number_of_jobs;
	float throughput;
	// Averages can be calculated from these
};


struct job job_queue[JOB_BUF_SIZE];
int job_q_index_location = -1;

// int a[50];
/*
 *  Command table.
 */
static struct {
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	/* commands: single command must end with \n */
	{ "?\n",	cmd_helpmenu },
	{ "h\n",	cmd_helpmenu },
	{ "help\n",	cmd_helpmenu },
	{ "r\n",	cmd_run },
	{ "run\n",	cmd_run },
	{ "list\n",	cmd_run },
	{ "fcfs\n",	fcfs },
	{ "sjf\n",	sjf },
	{ "priority\n",	priority },
	{ "q\n",	cmd_quit },
	{ "quit\n",	cmd_quit }
};

char ornela = 'o';

int fcfs(){

	printf("First come first serve.\n");
	struct Node* head = NULL;
	job_queue[0].id = 5;
	// printf("%d\n", job_queue[0].id );


	ornela = 's';
	// struct Node* head = NULL;
	// push(&head, 6);
	// push(&head, 6);
	// printf("%d\n", head->job->data);
}

int sjf(){
	printf("Shortest Job First.\n");
}

int priority(){
	printf("Priority.\n");
}

/*
 * The run command - submit a job.
 */
int cmd_run(int nargs, char **args) {
	// if (nargs != 4) {
	// 	printf("Usage: run <job> <time> <priority>\n");
	// 	return EINVAL;
	// }
    job_q_index_location = 0;

    /* Use execv to run the submitted job in AUbatch */
    printf("use execv to run the job in AUbatch.\n");
  	return 0; /* if succeed */
}

/*
 * Command line main loop.
 */
pthread_mutex_t job_queue_lock;  /* Lock for critical sections */
pthread_cond_t job_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t job_buf_not_empty; /* Condition variable for buf_not_empty */


int main()
{
	struct Perf_info c;
	char *buffer;
    size_t bufsize = 64;
    int  iret1, iret2;

    pthread_t sched_thread, dispatcher_thread; /* Two concurrent threads */
   	iret1 = pthread_create(&sched_thread, NULL, sched_function, NULL);
    iret2 = pthread_create(&dispatcher_thread, NULL, dispatch_function, NULL);
    pthread_mutex_init(&job_queue_lock, NULL);
    pthread_cond_init(&job_buf_not_full, NULL);
    pthread_cond_init(&job_buf_not_empty, NULL);

    // pthread_join(sched_thread, NULL);
    // pthread_join(dispatcher_thread, NULL); 

    buffer = (char*) malloc(bufsize * sizeof(char));
    if (buffer == NULL) {
		perror("Unable to malloc buffer");
		exit(1);
	}
 
    while (1) {
		printf("> [? for menu]: ");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
	}
        return 0;
}

void *sched_function(void *ptr) {

	printf("\nStarting Schedular\n");
    pthread_mutex_lock(&job_queue_lock);
    while (job_q_index_location == JOB_BUF_SIZE) {
        pthread_cond_wait(&job_buf_not_full, &job_queue_lock);
    }

    while (job_q_index_location == -1) {

    }
    printf("schedular: queue not full, signaling not empty\n");
    pthread_cond_signal(&job_buf_not_empty);
    pthread_mutex_unlock(&job_queue_lock);
}

void *dispatch_function(void *ptr) {
	printf("\nStarting Dispatcher\n");
	pthread_mutex_lock(&job_queue_lock);
    while (job_q_index_location == -1) {
        pthread_cond_wait(&job_buf_not_empty, &job_queue_lock);
    }

    printf("dispatcher: queue not full, signaling not empty\n");
    pthread_cond_signal(&job_buf_not_full);
    pthread_mutex_unlock(&job_queue_lock);
}


/*
 * Process a single command.
 */
int cmd_dispatch(char *cmd)
{
	time_t beforesecs, aftersecs, secs;
	u_int32_t beforensecs, afternsecs, nsecs;
	char *args[MAXMENUARGS];
	int nargs=0;
	char *word;
	char *context;
 	int i, result;

	for (word = strtok_r(cmd, " ", &context);
	     word != NULL;
	     word = strtok_r(NULL, " ", &context)) {

		if (nargs >= MAXMENUARGS) {
			printf("Command line has too many words\n");
			return E2BIG;
		}
		args[nargs++] = word;
	}

	if (nargs==0) {
		return 0;
	}

	for (i=0; cmdtable[i].name; i++) {
		if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)) {
			assert(cmdtable[i].func!=NULL);
            
            /*Qin: Call function through the cmd_table */
			result = cmdtable[i].func(nargs, args);
			return result;
		}
	}

	printf("%s: Command not found\n", args[0]);
	return EINVAL;
}

