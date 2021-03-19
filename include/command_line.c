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
	"run <j> <t> <p>:\tSubmit a job named <j>, CPU time <t> and priority <p>.      ",
	"test <b> <p> <n> <a>\n"
	"\t     <l> <min> <max>:\tTest the program with the given parameters.\n"
	"\t\t\t\twhere <b> is benchmark name, <p> is policy,\n"
	"\t\t\t\t<n> is num of jobs, <a> is arrival rate,\n"
	"\t\t\t\t<l> is priority levels, <min> <max> are min/max CPU time.",
	"list:\t\t\tList jobs in the queue.       ",
	"fcfs:\t\t\tChange scheduling policy to FCFS.       ",
	"sjf:\t\t\tChange scheduling policy to SJF.       ",
	"priority:\t\tChange scheduling policy to Priority.       ",
	"clear: \t\t\tClear the screen.       ",
	"help or ?: \t\tPrint help menu.              ",
	"quit: \t\t\tExit the program immediatly.              ",
	"quit -m:\t\tExit the program with exit mode m,\n"
	"\t\t\t\t-m can be -i for immediate termination,"
	"\n\t\t\t\tor -d for delayed termination.                ",
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

