
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[] ) {

	float cpu_time = atof(argv[1]);
	int sleep_time = (int) cpu_time;
	// printf("\nStarting execution of job for [%4.2f]\n", cpu_time);
	// printf("Sleeping for %d\n",sleep_time);
	sleep(sleep_time);
	// printf("finished Sleeping for %d\n",sleep_time);
	// printf("program run finished\n");
	exit(0);
}