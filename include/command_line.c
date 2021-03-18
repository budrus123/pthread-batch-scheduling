/*
 * COMP7500/7506
 * Project 3: commandline_parser.c
 *
 * This sample source code demonstrates how to:
 * (1) separate policies from a mechanism
 * (2) parse a commandline using getline() and strtok_r()
 * 
 * The sample code was derived from menu.c in the OS/161 project
 * 
 * Xiao Qin
 * Department of Computer Science and Software Engineering
 * Auburn University
 *
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "command_line.h"

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  4 
#define MAXCMDLINE   64 


/*
 * Display menu information
 */
void showmenu(const char *name, const char *x[])
{
	int ct, half, i;
	printf("%s\n", name);
	
	for (i=ct=0; x[i]; i++) {
		ct++;
	}
	half = (ct+1)/2;

	for (i=0; i<ct; i++) {
		printf("\t%s", x[i]);
		printf("\n");
	}

	printf("\n");
}

static const char *helpmenu[] = {
	"run <job> <time> <pri>: Submit a job named <job>,\n"
	"\t\t\t\texecution time <time>,\n\t\t\t\tpriority is <pri>.      ",
	"test <benchmark> <policy> <num_of_jobs> <arrival_rate>\n"
	"\t     <priority_levels> <min_CPU_time> <max_CPU_time>\n"
	"\t     Test the program with the given parameters.",
	"list:\t\tList jobs in the queue.       ",
	"fcfs:\t\tChange scheduling policy to FCFS.       ",
	"sjf:\t\tChange scheduling policy to SJF.       ",
	"priority:\tChange scheduling policy to Priority.       ",
	"clear: \t\tClear the screen.       ",
	"help or ?: \tPrint help menu.              ",
	"quit -<mode>:\tExit the program with exit mode <mode>,\n"
	"\t\t\t-<mode> can be -i for immediate termination,"
	"\n\t\t\tor -d for delayed termination.                ",
        /* Please add more menu options below */
	NULL
};

int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;
	help_invoked = 1;
	showmenu("Help menu:", helpmenu);
	return 0;
}

int clear_screen() {
	system("clear");
}

