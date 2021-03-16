struct Perf_info {
	time_t program_start_time;
	time_t program_end_time;
	double total_cpu_time;
	double total_waiting_time;
	double total_turnaround_time;
	int total_number_of_jobs;
	double throughput;
};