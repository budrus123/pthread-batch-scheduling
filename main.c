
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
#define JOB_BUF_SIZE 6

int head = 0;
int tail = 0;


int sjf();
int fcfs();
int priority();


void *sched_function( void *ptr ); 
void *dispatch_function( void *ptr );  
void print_job_info(struct job new_job);
void execute_job_process(struct job executing_job);
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

typedef enum { 
	FCFS, 
	SJF, 
	PRIORITY, 
	NONE
} Policy;

Policy policy = NONE;
int policy_change = 0;

void change_queue_to_fcfs(struct job job[], int count);
void change_queue_to_sjf(struct job job[], int count);
void change_queue_to_priority(struct job job[], int count);
void list_all_jobs();
void update_policy(Policy policy);


struct Perf_info {
	int total_cpu_time;
	int total_waiting_time;
	int total_turnaround_time;
	int total_number_of_jobs;
	float throughput;
	// Averages can be calculated from these
};


struct job job_queue[JOB_BUF_SIZE];
struct job completed_jobs[JOB_BUF_SIZE * 10];

struct job new_job;
struct job running_job;

int job_q_index_location = -1;
int completed_job_index = 0;

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
	{ "list\n",	list },
	{ "fcfs\n",	fcfs },
	{ "sjf\n",	sjf },
	{ "priority\n",	priority },
	{ "q\n",	cmd_quit },
	{ "quit\n",	cmd_quit }
};

int fcfs(){
	printf("Changing policy to FCFS.\n");
	policy_change = 1;
	policy = FCFS;
}

int sjf(){
	printf("Changing policy to SJF.\n");
	policy_change = 1;
	policy = SJF;
}

int priority(){
	printf("Changing policy to Priority.\n");
	policy_change = 1;
	policy = PRIORITY;
}


pthread_mutex_t job_queue_lock;  /* Lock for critical sections */
pthread_cond_t job_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t job_buf_not_empty; /* Condition variable for buf_not_empty */


int list(int nargs, char **args) {
	list_all_jobs();
}

int cmd_run(int nargs, char **args) {

	if (nargs != 4) {
		printf("Usage: run <job_name> <time> <priority>\n");
		return EINVAL;
	}

	char* name = args[1];
	float cpu_time = atof(args[2]);
	int priority = atoi(args[3]);

	strcpy(new_job.job_name, name);
	new_job.cpu_time = cpu_time;
	new_job.id = 17;
	new_job.priority = priority;
	new_job.finish_time = -1;
	new_job.arrival_time = time(0);
  	return 0; /* if succeed */
}



int main()
{

	policy_change = 0;
	struct Perf_info c;
	char *buffer;
	size_t bufsize = 64;
	int  iret1, iret2;
	new_job.id = -1;
    pthread_t sched_thread, dispatcher_thread; /* Two concurrent threads */
    printf("Starting Schedular....\n");
	iret1 = pthread_create(&sched_thread, NULL, sched_function, NULL);
	printf("Starting Dispatcher....\n");
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

	/*
 	* Command line main loop.
 	*/
	while (1) {
		printf("> [? for menu]: ");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
	}
	return 0;
}

void *sched_function(void *ptr) {

	while (1) {
		pthread_mutex_lock(&job_queue_lock);
		while (queue_full()) {
			printf("Buffer full");
			pthread_cond_wait(&job_buf_not_full, &job_queue_lock);
		}

		if (new_job.id != -1) {
			enqueue(new_job);
			printf("New job has been submitted\n");
			if (policy != FCFS) {
				update_policy(policy);
			}
			// printf("\nschedular: queue not empty, signaling not empty\n");
			// job_q_index_location += 1;
			// job_queue[job_q_index_location] = new_job;
			new_job.id = -1;
			pthread_cond_signal(&job_buf_not_empty);
		}
		// list_all_jobs();
		if (policy_change != -1) {
			update_policy(policy);
			policy_change = -1;
			// policy = NONE;
		}
		// while (job_q_index_location == -1) {
		// 	pthread_cond_wait(&job_buf_not_empty, &job_queue_lock);
		// }
		pthread_mutex_unlock(&job_queue_lock);

	}

}

void *dispatch_function(void *ptr) {
	while(1) {
		pthread_mutex_lock(&job_queue_lock);
		while (queue_empty()) {
			// printf("queu empty: sleeping, dispatcher: \n");
			pthread_cond_wait(&job_buf_not_empty, &job_queue_lock);
		}

		// printf("dispatcher dispatcher\n");


		// pthread_mutex_lock(&job_queue_lock);

		// TODO: return this
		struct job first_job = dequeue();
		// printf("got %f\n",first_job.cpu_time);
		running_job = first_job;

		pthread_mutex_unlock(&job_queue_lock);

		// pthread_t exec_thread; /* Two concurrent threads */
		// int th = pthread_create(&exec_thread, NULL, exec_thread_function, NULL);
		// pthread_join(exec_thread, NULL);

		pid_t pid = fork();

		switch (pid)
		{
		case -1:
		  /* Fork() has failed */
		  perror("fork");
		  break;
		case 0:
		  /* This is processed by the child */
		  execute_job_process(first_job);
		  puts("Uh oh! If this prints, execv() must have failed");
		  exit(0);
		  break;
		default:
		  /* This is processed by the parent */
		  wait(NULL);
		  running_job.id = -1;
		  break;
		}
	
		// print_job_info(first_job);
		// pthread_mutex_unlock(&job_queue_lock);
		// execute_job_process();
		// job_q_index_location -= 1;


		// printf("\ndispatcher: queue not empty\n");
		pthread_cond_signal(&job_buf_not_full);
	}

}

void list_all_jobs() {
	printf("Name\tCPU_Time\tPri\tArrival_time\tProgress\n");
	if (running_job.id != -1) {
		print_job_info(running_job);
		printf("\tRun\n");
	}
	int i = tail;
	while (i < head) {
		print_job_info(job_queue[i]);
		printf("\n");
		i++;
	}
}

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

void change_queue_to_priority(struct job temp_jobs[], int count) {
	int i, j;
	for (i = 0; i < count -1 ; i++) {
		for (j=0; j < count-i-1; j++) {
			if (temp_jobs[j].priority <temp_jobs[j+1].priority) {
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

int get_count_elements_in_queue() {
	if (head > tail) {
		return head - tail;
	} else {
		return JOB_BUF_SIZE - (tail - head);
	}
}

void *exec_thread_function(void *ptr) {
	sleep(30);
	printf("job job job\n");
}

void execute_job_process(struct job executing_job) {
	print_job_info(executing_job);
	float cpu_time = executing_job.cpu_time;
	char float_in_string[10];
	gcvt(cpu_time, 4, float_in_string);
	// printf("cput time strong is %s\n", float_in_string);
	// // printf("need to execute for %f\n",);
	// // printf("Name: %s",executing_job.job_name);
	char *my_args[3];  
  	my_args[0] = "./job_process";
  	my_args[1] = float_in_string;
  	my_args[2] = NULL;
  	execv("./job_process", my_args);

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

	printf("%s\t",new_job.job_name);
	printf("%4.2f\t\t",new_job.cpu_time);
	printf("%d\t",new_job.priority);
	char* arrive_time = ctime(&new_job.arrival_time);
	arrive_time[strlen(arrive_time)-5] = '\0';
	printf("%s",arrive_time+11);
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
		// print_job_info(tail_job);
		return tail_job;
	}
}

struct job enqueue(struct job new_job) {
	if (!queue_full()) {
		int next_position = get_next_position();
		job_queue[head] = new_job;
		head = next_position;
	}
}