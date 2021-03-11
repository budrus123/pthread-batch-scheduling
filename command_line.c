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
 * Compilation Instruction:
 * gcc commandline_parser.c -o commandline_parser
 * ./commandline_parser
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
		printf("\t\t%s", x[i]);
		printf("\n");
	}

	printf("\n");
}

static const char *helpmenu[] = {
	"[run] <job> <time> <priority>       ",
	"[list]: What the list does       ",
	"[fcfs]: Something Something       ",
	"[sjf]: Something Something       ",
	"[priority]: Something Something       ",
	"[test] <benchmark> <policy> <num_of_jobs> <priority_levels> <min_CPU_time> <max_CPU_time>",
	"[quit] Exit AUbatch                 ",
	"[help] Print help menu              ",
        /* Please add more menu options below */
	NULL
};

int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;

	showmenu("AUbatch help menu", helpmenu);
	return 0;
}

void clear_screen() {
	system("clear");
}