#include "job.h"
#include <time.h>
#include <string.h>


/*
* Function to print the job info
* Basically prints all the info of the job
* in the job struct.
*/

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



/*
* Function to execute the job process.
* This function recieves the job process
* and executes the batch_job process with the 
* cpu_time as a float so that the batch_job
* can wait for that amount of time.
*/

void execute_job_process(struct job executing_job) {
	float cpu_time = executing_job.cpu_time;
	char float_in_string[10];
	gcvt(cpu_time, 4, float_in_string);
	char *my_args[3];  
  	my_args[0] = "./batch_job";
  	my_args[1] = float_in_string;
  	my_args[2] = NULL;
  	execv("./batch_job", my_args);
}


/*
* Function that fills the job details, which are
* the turnaround time and the wait time.
*/

void fill_job_details(struct job* completed_job) {
	double difference = difftime((*completed_job).finish_time, (*completed_job).arrival_time);
	(*completed_job).turnaround_time = difference;
	(*completed_job).wait_time = (*completed_job).turnaround_time - (*completed_job).cpu_time;
}


/*
* Function to see if the queue is empty
* or not.
*/
int queue_empty() {
	return head == tail;
}

/*
* Function to see if the queue is full 
* or not.
*/
int queue_full() {
	int next_position = (head + 1) % JOB_BUF_SIZE;
	return next_position == tail;
}

/*
* Function to get the next available position
* in the circular queue.
*/
int get_next_position() {
	int next_position = (head + 1) % JOB_BUF_SIZE;
	return next_position;
}

/*
* Function to dequeue a job from the queue
* and then return.
*/
struct job dequeue() {
	if (!queue_empty()) {
		struct job tail_job = job_queue[tail];
		tail = (tail + 1 ) % JOB_BUF_SIZE;
		return tail_job;
	}
}

/*
* Function that takes a job and then 
* enqueues (adds) that job to the queue.
*/
struct job enqueue(struct job new_job) {
	if (!queue_full()) {
		int next_position = get_next_position();
		job_queue[head] = new_job;
		head = next_position;
	}
}


/*
* Function that returns the number of elements in the queue
*/
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
* Function to list all the jobs.
* This lists all the completed jobs, the currently running job
* and the jobs waiting in the queue.
*/

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


/*
* Function that gets the expected wait time for a 
* newly added job. This is done, by 
* computing the CPU time of all the jobs in the 
* queue and the currently running job and adding the all up.
*/
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

