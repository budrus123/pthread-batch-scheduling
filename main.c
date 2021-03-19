#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "command_line.h"
#include "job.h"
#include "policy.h"
#include "performance.h"

#define JOB_BUF_SIZE 50
#define EINVAL       1
#define E2BIG        2
#define MAXMENUARGS  9
#define MAXCMDLINE   64 
#define NEW_JOB_ID   1 

/*
* Global variables
*/
Policy policy = NONE;
int policy_change = 0;
int completed_job_index = 0;
int currently_executing = 0;
int head = 0;
int tail = 0;
char benchmark_name [30];
int help_invoked = 0;

/*
* Job queue which is a circular array that 
* is maintened by the program using a head and
* tail pointer
*/
struct job job_queue[JOB_BUF_SIZE];

/*
* Array of jobs that are completed
* The size of the array is 10 times the queue
* meaning that a person could run up to 500 jobs
* and store them when completed in this array. (if
* the queue size was 50)
*/
struct job completed_jobs[JOB_BUF_SIZE * 10];

// Variable to tell if the program is in test mode
int test_mode = 0;

// Global performance info variable
struct Perf_info performance_metrics;

// Variables to store a newly added job
// and the current running job
struct job new_job;
struct job running_job;

// Mutex to lock the queue
pthread_mutex_t job_queue_lock; 

// Mutex to lock the completed job queue (array)
pthread_mutex_t completed_job_queue_lock;

// Mutex to lock the new job variable 
// so it is only added to the queue after
// completion of the run command
pthread_mutex_t new_job_job_lock; 

pthread_cond_t job_buf_not_full; /* Condition variable for buf_not_full */
pthread_cond_t job_buf_not_empty; /* Condition variable for buf_not_empty */

// Function prototypes
void *scheduling_module( void *ptr ); 
void *dispatching_module( void *ptr );  
void compute_performance_measures();
void print_performance_measures();
float get_uniform_element(float min, float max, int elements_count, int position);
void initialize_global_variables();
void reset_program();
float random_in_range(float low, float high);
void change_queue_to_fcfs(struct job job[], int count);
void change_queue_to_sjf(struct job job[], int count);
void change_queue_to_priority(struct job job[], int count);
void update_policy(Policy policy);
void print_intro();
int bacnmark_exisits(char* benchmark_name);

/*
 *  Command table.
 */
static struct {
	const char *name;
	int (*func)(int nargs, char **args);
} cmdtable[] = {
	/* commands: single command must end with \n */
	{ "?\n",		cmd_helpmenu },
	{ "h\n",		cmd_helpmenu },
	{ "help\n",		cmd_helpmenu },
	{ "r",			run_job },
	{ "run",		run_job },
	{ "test",		test },
	{ "list\n",		list },
	{ "fcfs\n",		fcfs },
	{ "sjf\n",		sjf },
	{ "priority\n",	priority },
	{ "quit\n",		cmd_quit_immediate },
	{ "q",			cmd_quit },
	{ "quit",		cmd_quit },
	{ "clear\n", 	clear_screen },
	{NULL, NULL}

};

void print_intro() {
	printf("Pthread-based Batch Scheduling System.\n");
	printf("Type `help` to find more about the supported commands.\n");
}

/*
* Main function
* Main function creates the threads and runs them
* it also handles the command from the user.
* When in test mode, the command is disabled
*/

int main()
{
	system("clear");
	initialize_global_variables();
	reset_program();
	print_intro();
	char *buffer;
	size_t bufsize = 64;
	int  iret1, iret2;
    pthread_t sched_thread, dispatcher_thread; /* Two concurrent threads */
    // printf("[Starting Schedular ]\n");
	iret1 = pthread_create(&sched_thread, NULL, scheduling_module, NULL);
	// printf("[Starting Dispatcher]\n");
	iret2 = pthread_create(&dispatcher_thread, NULL, dispatching_module, NULL);

	// Initialization of all the mutexes and 
	// conditonal variables
	pthread_mutex_init(&job_queue_lock, NULL);
	pthread_mutex_init(&new_job_job_lock, NULL);
	pthread_mutex_init(&completed_job_queue_lock, NULL);
	pthread_cond_init(&job_buf_not_full, NULL);
	pthread_cond_init(&job_buf_not_empty, NULL);

	buffer = (char*) malloc(bufsize * sizeof(char));
	if (buffer == NULL) {
		perror("Unable to malloc buffer");
		exit(1);
	}

	/*
 	* Command line main loop.
 	*/
	while (1) {
		// Once the user invokes the help method
		// stop printing the [? for help]
		if (help_invoked)
			printf("> ");
		else
			printf("> [? for help]: ");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
		usleep(1000);
		while (test_mode){}
	}
	return 0;
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
	performance_metrics.total_number_of_jobs = completed_job_index;
	compute_performance_measures();
	print_performance_measures();
	printf("\n");
    exit(0);
}

/*
* Calling quit with no arguments
* wll just call the quit in immediate mode.
*/
int cmd_quit_immediate(int nargs, char **args) { 
	char *my_args[2];  
	char mode [5];
	strcpy(mode, "-i\n");
  	my_args[0] = "quit";
  	my_args[1] = mode;
	cmd_quit(2, my_args);
}


/*
* The list command.
* The list command just calls the list all jobs 
* function that is used throughout the program.
*/
int list(int nargs, char **args) {
	list_all_jobs();
}


/*
* Test function.
*
* Function to test the program by taking the folloiwng params,
* <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels> 
* <min_cpu_time> <max_cpu_time>.
*
* Note here that the benchmark is called real_job which utilizes the CPU
* there is also a  dummy batch_job program that will sleep for a certain 
* amount of time.
*
* For the CPU time and priority levels, a random number in the range of 
* min_cpu_time and max_cpu_time is generated and used as the CPU time.
* For priority, the same is done (from the range of 1 until the priority level
* given).
*
* For arrival rate, it controls the flow of jobs. For example, if the arrival
* rate was 1, this means that each second, exactly one job arrives, so after 
* each job submission, the test command sleeps for 1 second. If the arrival rate,
* was less than one second, say 0.1 job/second meaning that each 10 seconds, a single
* job arrives, then after each job submission, the test command sleeps for 1/arrival_rate.
* For example, if the arrival rate was 0.1, and 2 jobs were submitted, that means,
* the program needs 20 seconds to submit both jobs (or 10 since we need 1 sleep in between).
* After each job submission, the test command will sleep for 10 seconds (inverse of arrival rate).
*
* Note that a sepcial variable (flag) called test_mode was used to know
* when the program is running tests, thus, not printing anything during the tests
* and also not allowing the user to do anything when tests are running.
*/

int test(int nargs, char **args) {

	if (nargs != 8) {
		printf("Usage: test <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels>\n"
			"\t    <min_cpu_time> <max_cpu_time>\n");
		return EINVAL;
	}
	test_mode = 1;
	char* test_benchmark_name = args[1];
	char* policy_string = args[2];
	policy_string[strlen(policy_string)] = '\0';

	// Check to see which policy was passed
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

	// Make sure the test benchmark is actually there
	if (!bacnmark_exisits(test_benchmark_name)) {
		printf("\nProvided benchmark doesn't exisit. Check `benchmark` directory."
			"\nExample of available benchmark is `real_job` or `batch_job`.\n\n");
		test_mode = 0;
		return -1;
	}

	// Start the test on a fresh queue and no completed jobs
	reset_program();
	strcpy(benchmark_name, test_benchmark_name);
	time(&performance_metrics.program_start_time);
	int num_of_jobs = atoi(args[3]);
	float arrival_rate = atof(args[4]);
	int priority_levels = atoi(args[5]);
	float min_cpu_time = atof(args[6]);
	float max_cpu_time = atof(args[7]);
	int i = 0;
	printf("Starting tests....\n");

	// For loop to submit the jobs,
	// Jobs are submitted using the 
	// run command.
	for(i = 0; i < num_of_jobs; i++) {
		char *my_args[4];  

		/*
		* Using random cpu_time and priority within the input range
		*/
		int priority = (int) random_in_range(1, priority_levels);
		float cpu_time =random_in_range(min_cpu_time, max_cpu_time);

		/*
		* Using uniform cpu_time and priority within the input range
		* Default is above (random).
		*/
		// int priority = (int) get_uniform_element(1, priority_levels, num_of_jobs, i);
		// float cpu_time = get_uniform_element(min_cpu_time, max_cpu_time, num_of_jobs, i);

		char priority_string [5];
		sprintf(priority_string, "%ld" , priority);
		char float_in_string[10];
		gcvt(cpu_time, 4, float_in_string);		
	  	my_args[0] = "run";
	  	my_args[1] = "test_r";
	  	my_args[2] = float_in_string;
	  	my_args[3] = priority_string;
		run_job(4, my_args);

		/*
		* Use arrival rate to know when to sleep.
		* if arrival rate is less than one, take the inverse
		* and sleep that amount after each job.
		*
		* If rate > 1 then after rate multiples of jobs added,
		* sleep for a second. For example if the rate is 5 jobs/second
		* then after adding 5, 10, 15, and so on jobs, the program sleeps
		* for a second.
		*/

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
	system("clear");
	printf("\n");
	list_all_jobs();
	int count_queue = get_count_elements_in_queue();
	if (count_queue > 0 || running_job.id != -1) {
		printf("\nPending completion of running programs...\n");
	}
	int refresh_counter = 1;

	// Refresh function for listing and clearing the screen.
	// Until all jobs finish.
	while(get_count_elements_in_queue() > 0 || running_job.id != -1) {
		if (refresh_counter % 10000000 == 0) {
			system("clear");
			list_all_jobs();
			usleep(10);			
		}
		refresh_counter++;
	} 
	system("clear");
	usleep(10);	
	list_all_jobs();	
	time(&performance_metrics.program_end_time);
	performance_metrics.total_number_of_jobs = completed_job_index;
	compute_performance_measures();
	print_performance_measures();
	printf("\n");
	// After tests end, reset test mode 
	// and all the other global vars like the queue
	// and the completed jobs
	test_mode = 0;
	initialize_global_variables();
	reset_program();
}


/*
* Function to make sure that the benchmark that will execute
* actually exisits.
*/
int bacnmark_exisits(char* benchmark_name) {
	char directory[50];
	char extension[10];
	strcpy(directory, "benchmark/");
	strcpy(extension, ".o");
	strcat(directory, benchmark_name);
	strcat(directory, extension);
	FILE *fptr = fopen(directory, "r");
	if (fptr == NULL) {
		return 0;
	}
	fclose(fptr);
	return 1;
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
	// To make sure new job id is not -1
	new_job.id = NEW_JOB_ID; 
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
	strcpy(benchmark_name, "batch_job");
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
	// help_invoked = 0;
	head = 0;
	tail = 0;
	completed_job_index = 0;
	currently_executing = 0;
	time(&performance_metrics.program_start_time);
	performance_metrics.total_waiting_time = 0;
	performance_metrics.total_cpu_time = 0;
	performance_metrics.total_turnaround_time = 0;
	performance_metrics.total_number_of_jobs = 0;
}


/*
* Scheduling module thread.
* 
* The scheduling module is in-charge of adding new jobs to the queue
* as well as updating the queue to reflect scheduling policy changes.
*
* New jobs are added to the queue using a variable called 'new_job' that 
* is set and synchronized with the 'job_run' function/command. Initally,
* this job id is -1, when a new job is added it is set with a specific ID
* to let the schedular know that this is a real new job. The schedular 
* then takes this job and adds it to the queue. After it has been added,
* it gets reset. Note that the job is added in accordance to the current
* scheduling policy.
*
* In addition, the schedular makes sure that the policy changes are
* reflected using a policy_change flag to call the needed methods to 
* sort the queue depending on the policy.
*/

void *scheduling_module(void *ptr) {

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
				print_policy(policy);
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


/*
* Dispatching module thread.
*
* The dispatching module is in-charge of taking the ready jobs
* at the beginning of the job queue and execute them. The dispatching 
* module first waits for the job_queue to not be empty, then the first 
* job is retreived from the queue so it can be executed. The start time 
* for this job is then set. Then a new process, using fork(), is 
* created to run the batch program.
*
* After the child (batch program) has completed, the parent (dispatching module),
* sets the finish time for this job and it computes all the needed times for this 
* job, like wait and turn-around time.
*
* Note that job execution is carried out by the 'execute_job_process' function
* in the job.c file.
*
* At the end, the job is added to an array of jobs called 'completed_jobs' to
* keep track of all the completed jobs.
*/

void *dispatching_module(void *ptr) {
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

	// Remove the new line from the entered
	// command for pritning.
	char unknown_command [30];
	strcpy(unknown_command, args[0]);
	unknown_command[strlen(unknown_command)-1] = '\0';
	printf("Command [ %s ] not found\n", unknown_command);
	return EINVAL;
}
