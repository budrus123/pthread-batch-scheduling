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

	printf("\n");
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
	"[run]: run a certain job. Run <name> <time> <priority>       ",
	"[list]: List jobs in the queue       ",
	"[fcfs]: Change scheduling policy to FCFS       ",
	"[sjf]: Change scheduling policy to SJF       ",
	"[priority]: Change scheduling policy to Priority       ",
	"[clear]: clear the screen       ",
	"[test]: <benchmark> <policy> <num_of_jobs> <arrival_rate> <priority_levels> <min_CPU_time> <max_CPU_time>",
	"[quit]: -<exit_mode>. Exit mode can be -i or -d                 ",
	"[help]: Print help menu              ",
        /* Please add more menu options below */
	NULL
};

int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;

	showmenu("Help menu:", helpmenu);
	return 0;
}

void clear_screen() {
	system("clear");
}

