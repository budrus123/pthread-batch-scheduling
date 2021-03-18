#define JOB_BUF_SIZE 50

struct Perf_info {
	time_t program_start_time;
	time_t program_end_time;
	double total_cpu_time;
	double total_waiting_time;
	double total_turnaround_time;
	int total_number_of_jobs;
	double throughput;
};

void print_performance_measures();
void compute_performance_measures();

extern struct Perf_info performance_metrics;
extern int completed_job_index;
extern pthread_mutex_t completed_job_queue_lock;