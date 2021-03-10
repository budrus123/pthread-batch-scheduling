
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[] ) {

	float cpu_time = atof(argv[1]);
	printf("In dummy %f\n", cpu_time);
	// int i = 10;
	sleep(cpu_time);
	printf("program run finished\n");
	return 1;
}