#include <stdlib.h>
#include <stdio.h>
// #include <unistd.h>
#include <time.h>

int main(int argc, char *argv[] ) {

	float cpu_time = atof(argv[1]);
	float sleep_time = cpu_time * 1000000;
	time_t start_time, current_time;
	time(&start_time);
	double difference = 0;
	// printf("started cpu time is %f:\n", cpu_time);
	while (difference < cpu_time) {
		time(&current_time);
		difference = difftime(current_time, start_time);
	}
	// printf("differnce is %f\n", difference);
	// printf("ended:\n");
	// usleep(sleep_time);
	exit(0);
}