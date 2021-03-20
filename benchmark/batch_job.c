#include <stdlib.h>
#include <stdio.h>
// #include <unistd.h>
#include <time.h>

int main(int argc, char *argv[] ) {

	/*
	* Real job benchmark.
	* Since time gives us seconds, then this is 
	* a manual way to handle the CPU time.
	* The program counts seconds until the difference
	* between the current time and the start time
	* has reached the CU time.
	*
	* After that, the remaining fraction, for example,
	* if we have 3.5 as CPU time, the remaining 0.5 seconds
	* is used to create a long loop that runs for around
	* 0.5 seconds.
	*/
	
	float cpu_time = atof(argv[1]);
	float sleep_time = cpu_time * 1000000;
	time_t start_time, current_time;
	time(&start_time);
	double difference = 0;
	while (difference < (int)cpu_time) {
		time(&current_time);
		difference = difftime(current_time, start_time);
	}

	float remaining_fraction = cpu_time - (int) cpu_time;
	int one_sec_loop_counter = 400000000;
	int remaining_counter = one_sec_loop_counter * remaining_fraction;
	int i = 0;

	while (i < remaining_counter) {
		i++;
	}
	exit(0);
}