
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[] ) {

	float cpu_time = atof(argv[1]);
	// printf("\nStarting execution of job for [%4.2f]\n", cpu_time);
	sleep(cpu_time);
	// printf("program run finished\n");
	return 1;
}