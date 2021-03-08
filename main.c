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
#include <time.h>
// #include "job_queue.h"

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  4 
#define MAXCMDLINE   64 
#define JOB_BUF_SIZE 3

int head = 0;
int tail = 0;


int sjf();
int fcfs();
int priority();

void *sched_function( void *ptr ); 
void *dispatch_function( void *ptr );  
void print_job_info(struct job new_job);
void execute_dummy();
void *exec_thread_function(void *ptr);


int queue_full();
int get_next_position();

struct job dequeue();

struct job enqueue(struct job new_job);

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
struct job new_job;
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
	{ "r",	cmd_run },
	{ "run",	cmd_run },
	{ "list",	cmd_run },
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
 * Command line main loop.
 */
pthread_mutex_t job_queue_lock;  /* Lock for critical sections */
pthread_cond_t job_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t job_buf_not_empty; /* Condition variable for buf_not_empty */


int cmd_run(int nargs, char **args) {
	if (nargs != 4) {
		printf("Usage: run <job_name> <time> <priority>\n");
		return EINVAL;
	}
	// pthread_mutex_lock(&job_queue_lock);

	// job_q_index_location += 1;

	// struct job new_job;
	char* name = args[1];
	float cpu_time = atof(args[2]);
	int priority = atoi(args[3]);

	strcpy(new_job.job_name, name);
	new_job.cpu_time = cpu_time;
	new_job.id = 17;
	new_job.priority = priority;
	new_job.finish_time = -1;
	new_job.arrival_time = time(0);
	// print_job_info(new_job);

	// job_queue[job_q_index_location] = new_job;
	// pthread_cond_signal(&job_buf_not_empty);
	// pthread_mutex_unlock(&job_queue_lock);
  	return 0; /* if succeed */
}


int main()
{
	struct Perf_info c;
	char *buffer;
	size_t bufsize = 64;
	int  iret1, iret2;
	new_job.id = -1;
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
	// while(1) {
	// 	printf("yolo\n");
	// }
	while (1) {
		// printf("\nStarting Schedular\n");
		// printf("%d ", job_q_index_location);
		pthread_mutex_lock(&job_queue_lock);
		while (queue_full()) {
			printf("Buffer full");
			pthread_cond_wait(&job_buf_not_full, &job_queue_lock);
		}

		if (new_job.id != -1) {
			enqueue(new_job);
			printf("New job has been submitted\n");
			printf("\nschedular: queue not empty, signaling not empty\n");
			// job_q_index_location += 1;
			// job_queue[job_q_index_location] = new_job;
			new_job.id = -1;
			pthread_cond_signal(&job_buf_not_empty);
		}
		// while (job_q_index_location == -1) {
		// 	pthread_cond_wait(&job_buf_not_empty, &job_queue_lock);
		// }
		pthread_mutex_unlock(&job_queue_lock);

	}

}

void *dispatch_function(void *ptr) {
	while(1) {
		// printf("\nStarting Dispatcher\n");
		pthread_mutex_lock(&job_queue_lock);
		while (queue_empty()) {
			printf("queu empty: sleeping, dispatcher: \n");
			pthread_cond_wait(&job_buf_not_empty, &job_queue_lock);
		}
		printf("dispatcher dispatcher\n");
		// pthread_mutex_lock(&job_queue_lock);
		struct job first_job = dequeue();
		pthread_mutex_unlock(&job_queue_lock);
		pthread_t exec_thread; /* Two concurrent threads */
		int th = pthread_create(&exec_thread, NULL, exec_thread_function, NULL);
		pthread_join(exec_thread, NULL);
		// print_job_info(first_job);
		// pthread_mutex_unlock(&job_queue_lock);
		// execute_dummy();
		// job_q_index_location -= 1;
		printf("\ndispatcher: queue not empty\n");
		pthread_cond_signal(&job_buf_not_full);
	}

}

void *exec_thread_function(void *ptr) {
	sleep(10);
	printf("job job job\n");
}

void execute_dummy() {
	char *my_args[5];
  	pid_t pid;
  
  	my_args[0] = "./dummy";
  	my_args[1] = "-help";
  	my_args[2] = "-setup";
  	my_args[3] = NULL;
  	execv("./dummy", my_args);
  	printf("fuck my life has finished\n");
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

void print_job_info(struct job new_job){

	printf("\n\nPrinting job info:\n");
	printf("name: %s\n",new_job.job_name);
	printf("priority: %d\n",new_job.priority);
	char* arrive_time = ctime(&new_job.arrival_time);
	arrive_time[strlen(arrive_time)-1] = '\0';
	printf("arrival time: %s\n",arrive_time);
	printf("cpu time: %f\n\n\n",new_job.cpu_time);
	sleep(5);
}

int queue_empty() {
	return head == tail;
}

int queue_full() {
	int next_position = (head + 1) % JOB_BUF_SIZE;
	// printf("next poisition is %d\n",next_position);
	// printf("tail is %d\n",tail);
	return next_position == tail;
}

int get_next_position() {
	int next_position = (head + 1) % JOB_BUF_SIZE;
	return next_position;
}

struct job dequeue() {
	if (!queue_empty()) {
		struct job tail_job = job_queue[tail];
		tail = (tail + 1 ) % JOB_BUF_SIZE;
	}
}

struct job enqueue(struct job new_job) {
	if (!queue_full()) {
		int next_position = get_next_position();
		job_queue[next_position] = new_job;
		head = next_position;
	}
}