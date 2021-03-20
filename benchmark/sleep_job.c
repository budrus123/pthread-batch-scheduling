
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[] ) {

	float cpu_time = atof(argv[1]);
	float sleep_time = cpu_time * 1000000;
	usleep(sleep_time);
	exit(0);
}