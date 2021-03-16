#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "command_line.h"
#include <time.h>
#include "job.h"
#include "policy.h"
#include "performance.h"
#include "globals.h"

#define JOB_BUF_SIZE 20


/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  9
#define MAXCMDLINE   64 

int sjf();
int fcfs();
int priority();
int test();

void *sched_function( void *ptr ); 
void *dispatch_function( void *ptr );  
void *exec_thread_function(void *ptr);
void compute_performance_measures();
void print_performance_measures();
int queue_full();
int get_next_position();
float get_uniform_element(float min, float max, int elements_count, int position);
void initialize_global_variables();
void reset_program();


struct job dequeue();

struct job enqueue(struct job new_job);

struct Workload_data {
	int number_of_jobs;
	float arrival_rate;
	int min_cpu_time;
	int max_cpu_time;
};

float random_in_range(float low, float high);
void change_queue_to_fcfs(struct job job[], int count);
void change_queue_to_sjf(struct job job[], int count);
void change_queue_to_priority(struct job job[], int count);
void list_all_jobs();
void update_policy(Policy policy);
void fill_job_details(struct job* completed_job);
int get_expected_wait_time();
void print_policy();

void execute_job_process(struct job executing_job);

void print_job_info(struct job new_job);

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
	{ "r",	run_job },
	{ "run",	run_job },
	{ "test",	test },
	{ "list\n",	list },
	{ "fcfs\n",	fcfs },
	{ "sjf\n",	sjf },
	{ "priority\n",	priority },
	{ "q",	cmd_quit },
	{ "quit",	cmd_quit },
	{ "clear\n", clear_screen },
	{NULL, NULL}

};

int fcfs(){
	pthread_mutex_lock(&job_queue_lock);
	int count_queue = get_count_elements_in_queue();
	printf("Scheduling policy is switched to FCFS." 
		" All the %d waiting jobs have been rescheduled.\n", count_queue);	
	policy_change = 1;
	policy = FCFS;
	pthread_mutex_unlock(&job_queue_lock);

}

int sjf(){
	pthread_mutex_lock(&job_queue_lock);
	int count_queue = get_count_elements_in_queue();
	printf("Scheduling policy is switched to SJF." 
		" All the %d waiting jobs have been rescheduled.\n", count_queue);	
	policy_change = 1;
	policy = SJF;
	pthread_mutex_unlock(&job_queue_lock);
}

int priority(){
	pthread_mutex_lock(&job_queue_lock);
	int count_queue = get_count_elements_in_queue();
	printf("Scheduling policy is switched to Priority." 
		" All the %d waiting jobs have been rescheduled.\n", count_queue);	
	policy_change = 1;
	policy = PRIORITY;
	pthread_mutex_unlock(&job_queue_lock);
}

/*
 * The quit command.
 * The quit command can be used to terminate the program.
 * There are two modes for the quit command. The first mode
 * is quitting immediatly, this means that the pending jobs
 * are dropped and not executed.
 * The second mode is to wait for the completion of the
 * pending programs. For this mode use <-d>
 */

int cmd_quit(int nargs, char **args) {
	if (nargs != 2) {
		printf("Usage: %s -<quit_mode>\n", args[0]);
		return EINVAL;
	}
	char* quit_type = args[1];
	quit_type[strlen(quit_type)-1] = '\0';
	if(strcmp(quit_type, "-i") == 0) {
		printf("Quitting in immediate mode.\n");
	} 
	else if( strcmp(quit_type, "-d") == 0) {
		int count_queue = get_count_elements_in_queue();
		if (count_queue > 0 || running_job.id != -1) {
			printf("Pending completion of running programs...\n");
		}
		// Wait for elements in the queue to all finish
		while(get_count_elements_in_queue() > 0 || running_job.id != -1) {
		} 
	}
	else {
		printf("Unsupported quitting mode [%s]\n", quit_type);
		return 0;
	}

	time(&performance_metrics.program_end_time);
	printf("\n--------------------------------------------------------\n");
	printf("\t\tPerformance info below\n");
	printf("--------------------------------------------------------\n");
	performance_metrics.total_number_of_jobs = completed_job_index;
	compute_performance_measures();
	print_performance_measures();
	printf("\n");
    exit(0);
}

/*
* The list command.
* The list command just calls the list all jobs 
* function that is used throughout the program.
*/
int list(int nargs, char **args) {
	list_all_jobs();
}

int test(int nargs, char **args) {
	reset_program();
	if (nargs != 8) {
		printf("Usage: test <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels>\n"
			"\t    <min_cpu_time> <max_cpu_time>\n");
		return EINVAL;
	}
	test_mode = 1;

	char* benchmark_name = args[1];

	char* policy_string = args[2];
	policy_string[strlen(policy_string)] = '\0';

	if(strcmp(policy_string, "fcfs") == 0) {
		policy = FCFS;
	} else if(strcmp(policy_string, "sjf") == 0) {
		policy = SJF;
	} else if(strcmp(policy_string, "priority") == 0) {
		policy = PRIORITY;
	} else {
		printf("Unsupported scheduling mode [%s]\n", policy_string);
		test_mode = 0;
		return -1;
	}

	time(&performance_metrics.program_start_time);
	int num_of_jobs = atoi(args[3]);
	float arrival_rate = atof(args[4]);
	int priority_levels = atoi(args[5]);
	float min_cpu_time = atof(args[6]);
	float max_cpu_time = atof(args[7]);
	int i = 0;
	printf("Starting tests....\n");
	for(i = 0; i < num_of_jobs; i++) {
		char *my_args[4];  

		int priority = (int) random_in_range(1, priority_levels);
		float cpu_time =random_in_range(min_cpu_time, max_cpu_time);

		// int priority = (int) get_uniform_element(1, priority_levels, num_of_jobs, i);
		// float cpu_time = get_uniform_element(min_cpu_time, max_cpu_time, num_of_jobs, i);

		// priority is the int
		char priority_string [5];
		sprintf(priority_string, "%ld" , priority);

		char float_in_string[10];
		gcvt(cpu_time, 4, float_in_string);		
	  	// my_args[1] = float_in_string;


	  	my_args[0] = "run";
	  	my_args[1] = "test_r";
	  	my_args[2] = float_in_string;
	  	my_args[3] = priority_string;
		run_job(4, my_args);

		if (arrival_rate < 1) {
			double rate_to_seconds = (int) 1 / arrival_rate;
			sleep((int)rate_to_seconds);
		} else {
			if (i % (int) arrival_rate == 0) {
				sleep(1);
			}	
		}
		system("clear");
		list_all_jobs();
		usleep(1000);
		
	}

	printf("\n");
	list_all_jobs();
	int count_queue = get_count_elements_in_queue();
	if (count_queue > 0 || running_job.id != -1) {
		printf("\nPending completion of running programs...\n");
	}
	int refresh_counter = 1;
	while(get_count_elements_in_queue() > 0 || running_job.id != -1) {
		if (refresh_counter % 10000000 == 0) {
			system("clear");
			list_all_jobs();
			usleep(10);			
		}
		refresh_counter++;
	} 
	system("clear");
	list_all_jobs();	
	time(&performance_metrics.program_end_time);
	printf("\n--------------------------------------------------------\n");
	printf("\t\tPerformance info below\n");
	printf("--------------------------------------------------------\n");
	performance_metrics.total_number_of_jobs = completed_job_index;
	compute_performance_measures();
	print_performance_measures();
	printf("\n");
    // exit(0);

	test_mode = 0;
}


/*
* Function that returns a uniform element in a uniform distribution
* depending on the min and max and the position of this element.
*/
float get_uniform_element(float min, float max, int elements_count, int position) {
	float step = (max - min) / elements_count;
	return (min + (position * step));
}

/*
* Function that returns a random number in a certain range.
*/
float random_in_range(float low, float high) {
	// int random = (rand() % (high - low + 1)) + 1;
	// return random;
	float scale = rand() / (float) RAND_MAX;
	float random = low + scale * (high - low);
	return random;  
}


/*
* Run job function.
* Function that takes the 4 arguments needed to run the job.
* The first argument is the job name string, the second argument,
* is the CPU time of the job and the last argument is the priority
* level of that job.
*/

int run_job(int nargs, char **args) {

	if (nargs != 4) {
		printf("Usage: run <job_name> <time> <priority>\n");
		return EINVAL;
	}

	// Mutex to lock the new_job element
	// So when it is read in the schedular,
	// complete veersion is read.
	pthread_mutex_lock(&new_job_job_lock);
	char* name = args[1];
	float cpu_time = atof(args[2]);
	int priority = atoi(args[3]);

	strcpy(new_job.job_name, name);
	new_job.cpu_time = cpu_time;
	new_job.id = 17;
	new_job.priority = priority;
	time_t arrival;
	time(&arrival);
	new_job.arrival_time = arrival;
	pthread_mutex_unlock(&new_job_job_lock);
  	return 0; /* if succeed */
}


/*
* Initialize golbal variables function.
* Function to initialize all the golobal variables
* of the program.
*/

void initialize_global_variables() {

	srand(time(0));
	running_job.id = -1;
	policy_change = -1;
	policy = FCFS;
	new_job.id = -1;

}

/*
* Reset program function.
* Function used in resetting the main program
* variables. This resets the queue and the 
* completed job array.
*/

void reset_program() {

	head = 0;
	tail = 0;
	completed_job_index = 0;
	currently_executing = 0;
	time(&performance_metrics.program_start_time);

}

int main()
{
	initialize_global_variables();
	reset_program();
	struct Perf_info c;
	char *buffer;
	size_t bufsize = 64;
	int  iret1, iret2;
    pthread_t sched_thread, dispatcher_thread; /* Two concurrent threads */
    printf("[Starting Schedular ]\n");
	iret1 = pthread_create(&sched_thread, NULL, sched_function, NULL);
	printf("[Starting Dispatcher]\n");
	iret2 = pthread_create(&dispatcher_thread, NULL, dispatch_function, NULL);

	pthread_mutex_init(&job_queue_lock, NULL);
	pthread_mutex_init(&new_job_job_lock, NULL);
	pthread_mutex_init(&completed_job_queue_lock, NULL);
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
		usleep(1000);
		while (test_mode){}
	}
	return 0;
}

void *sched_function(void *ptr) {

	while (1) {
		pthread_mutex_lock(&job_queue_lock);
		while (queue_full()) {
			printf("Buffer full: waiting for completion of other jobs.\n"
				"Can't add any other jobs.");
			pthread_cond_wait(&job_buf_not_full, &job_queue_lock);
		}
		pthread_mutex_lock(&new_job_job_lock);
		if (new_job.id != -1) {
			enqueue(new_job);
			pthread_cond_signal(&job_buf_not_empty);		
			pthread_mutex_unlock(&job_queue_lock);
			if (policy != FCFS) {
				pthread_mutex_lock(&job_queue_lock);
				update_policy(policy);
				pthread_mutex_unlock(&job_queue_lock);
			}
			if (!test_mode) {
				printf("\nJob %s was submitted\n", new_job.job_name);
				int current_cpu_time = new_job.cpu_time;
				int count_of_jobs = get_count_elements_in_queue();
				printf("Total number of jobs in the queue: %d ", count_of_jobs);
				if (currently_executing) {
					printf("[1 running]\n");
				} else{
					printf("\n");
				}
				printf("Expected waiting time: %d\n", get_expected_wait_time());
				printf("Scheduling policy: ");
				print_policy();
				printf("\n\n");
			}
			new_job.id = -1;
		}
		if (policy_change != -1) {
			update_policy(policy);
			policy_change = -1;
		}
		pthread_mutex_unlock(&new_job_job_lock);
		pthread_mutex_unlock(&job_queue_lock);

	}

}
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

int get_expected_wait_time() {
	int expected_time = 0;

	int i = tail;
	while (i < head - 1) {
		expected_time += (int) job_queue[i].cpu_time;
		i++;
	}
	if (currently_executing = 1) {
		expected_time += running_job.cpu_time;
	}
	return expected_time;
}

void *dispatch_function(void *ptr) {
	while(1) {
		pthread_mutex_lock(&job_queue_lock);
		while (queue_empty()) {
			pthread_cond_wait(&job_buf_not_empty, &job_queue_lock);
		}
		struct job first_job = dequeue();
		currently_executing = 1;
		time(&first_job.start_time);
		running_job = first_job;
		pthread_mutex_unlock(&job_queue_lock);
		pid_t pid = fork();

		switch (pid) {
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
		default:;
		  /* This is processed by the parent */
			int return_status;
			waitpid(pid, &return_status, 0);
			// printf("job has finished\n");
			currently_executing = 0;
			time(&first_job.finish_time);
			fill_job_details(&first_job);
			running_job.id = -1;
			pthread_mutex_lock(&completed_job_queue_lock);
			completed_jobs[completed_job_index++] = first_job;

			pthread_mutex_unlock(&completed_job_queue_lock);
			break;
		}
		pthread_cond_signal(&job_buf_not_full);
	}

}

void fill_job_details(struct job* completed_job) {

	// struct tm* timeinfo;
	// timeinfo = localtime(&completed_job.arrival_time);
	// printf("local is %s\n",asctime(timeinfo) );

	// printf("filling fillings\n");
	// char* arrive_time = ctime(&completed_job.arrival_time);
	// arrive_time[strlen(arrive_time)-1] = '\0';
	// printf("Job arrived at: %s\n",arrive_time);

	// char* finish_time = ctime(&completed_job.finish_time);
	// finish_time[strlen(finish_time)-1] = '\0';
	// // printf("Job finished at: %s\n",finish_time);

	double difference = difftime((*completed_job).finish_time, (*completed_job).arrival_time);
	(*completed_job).turnaround_time = difference;
	(*completed_job).wait_time = (*completed_job).turnaround_time - (*completed_job).cpu_time;

	// printf("turnaround_time time is: %f\n",(*completed_job).turnaround_time);
	// printf("wait time is: %f\n",(*completed_job).wait_time);

}
void print_performance_measures() {
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

void compute_performance_measures() {
	pthread_mutex_lock(&completed_job_queue_lock);
	if (completed_job_index == 0) {
		printf("No jobs have completed, no info to display.\n");
		return;
	}

	int total_number_of_jobs = completed_job_index;
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

void list_all_jobs() {
	int count_of_jobs = get_count_elements_in_queue();
	printf("Total number of jobs in the queue: %d\n", count_of_jobs);
	if (!queue_empty() || running_job.id != -1 || completed_job_index != 0) {
		printf("-------------------------------------------------------------------------\n");
		printf("Name\tCPU_Time\tPri\tArrival_time\tStart_time\tProgress\n");
		printf("-------------------------------------------------------------------------\n");
		int completed_counter = 0;
		while(completed_counter < completed_job_index) {
			print_job_info(completed_jobs[completed_counter]);
			printf("\tCompleted\n");
			completed_counter++;
		}
		if (running_job.id != -1) {
			print_job_info(running_job);
			printf("\tRunning\n");
		}
		int i = tail;
		int j = 0;
		while (j < count_of_jobs) {
			print_job_info(job_queue[i]);
			printf("\n");
			i = (i + 1) % JOB_BUF_SIZE;
			j++;
		}
	}
	else{
		printf("No jobs pending execution.\n");
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

void print_job_info(struct job new_job){
	printf("%s\t",new_job.job_name);
	printf("%4.2f\t\t",new_job.cpu_time);
	printf("%d\t",new_job.priority);
	char* arrive_time = ctime(&new_job.arrival_time);
	arrive_time[strlen(arrive_time)-5] = '\0';
	printf("%s\t",arrive_time+11);

	// check if did not start yet, to handle jobs that still 
	// don't have a start time
	time_t reference_time = time(0);
	double difference = difftime(reference_time, new_job.start_time);
	if ( difference > 16158531 ){
		printf("  [NA]");
	} else {
		char* start_time = ctime(&new_job.start_time);
		start_time[strlen(start_time)-5] = '\0';
		printf("%s",start_time+11);
	}

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

int get_count_elements_in_queue() {
	if (queue_empty())
		return 0;
	if (head > tail) {
		return head - tail;
	} else {
		return JOB_BUF_SIZE - (tail - head);
	}
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

int queue_empty() {
	return head == tail;
}

int queue_full() {
	int next_position = (head + 1) % JOB_BUF_SIZE;
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