
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[] ) {

	float cpu_time = atof(argv[1]);
	float sleep_time = cpu_time * 1000000;
	// printf("\nStarting execution of job for [%4.2f]\n", cpu_time);
	// printf("Sleeping for %d\n",sleep_time);
	usleep(sleep_time);
	// printf("finished Sleeping for %d\n",sleep_time);
	// printf("program run finished\n");
	exit(0);
}