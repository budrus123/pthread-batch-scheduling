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
	"help -c: \t\tPrint help menu for command c              ",
	"quit: \t\t\tExit the program immediatly.              ",
	"quit -m:\t\tExit the program with exit mode m,\n"
	"\t\t\t\t-m can be -i for immediate termination,"
	"\n\t\t\t\tor -d for delayed termination.                ",
        /* Please add more menu options below */
	NULL
};


/*
* Function to display the general help menu
*/
int cmd_helpmenu(int n, char **a)
{
	(void)n;
	(void)a;
	help_invoked = 1;
	showmenu("Help menu:", helpmenu);
	return 0;
}

/*
* Function to display help menu for specific command
* using a -command flag with the help command.
*
* Example usage: help -test
*/
int cmd_helpmenu_option(int nargs, char **args)  {
	if (nargs != 2) {
		printf("Usage: %s -command\n", args[0]);
		return EINVAL;
	}
	char* quit_type = args[1];
	quit_type[strlen(quit_type)-1] = '\0';
	if(strcmp(quit_type, "-run") == 0 || strcmp(quit_type, "-r") == 0) {
		char *msg = "\nThe run command.\nThe run command is used to run a certain job.\n"
		"Usage: run <j> <t> <p>:\tSubmit a job named <j>, CPU time <t> and priority <p>.\n";
		show_help_menu_with_text(msg);
	} 
	else if( strcmp(quit_type, "-test") == 0) {
		char *msg = "\nThe Test command.\nThe test command is used to test the program"
		" under a certain workload.\n"
		"Usage: test <b> <p> <n> <a><l> <min> <max>:"
		"\n\tTest the program with the given parameters.\n"
		"\twhere <b> is benchmark name, <p> is policy,\n"
		"\t<n> is num of jobs, <a> is arrival rate,\n"
		"\t<l> is priority levels, <min> <max> are min/max CPU time.\n";
		show_help_menu_with_text(msg);
	}
	else if( strcmp(quit_type, "-fcfs") == 0) {
		char *msg = "\nThe fcfs command.\nThe fcfs command is used to change the scheduling"
		"\npolicy to first-come first-serve.\n";
		show_help_menu_with_text(msg);
	}
	else if( strcmp(quit_type, "-sjf") == 0) {
		char *msg = "\nThe sjf command.\nThe sjf command is used to change the scheduling"
		"\npolicy to shortest job first.\n";
		show_help_menu_with_text(msg);
	}
	else if( strcmp(quit_type, "-priority") == 0) {
		char *msg = "\nThe priority command.\nThe fcfs command is used to change the scheduling"
		"\npolicy to priority.\n";
		show_help_menu_with_text(msg);
	}
	else if( strcmp(quit_type, "-list") == 0) {
		char *msg = "\nThe list command.\nThe list command is used to list all completed,"
		"\nrunning, and pending jobs in the system.\n";
		show_help_menu_with_text(msg);
	}
	else if( strcmp(quit_type, "-quit") == 0 || strcmp(quit_type, "-q") == 0) {
		char *msg = "\nThe quit command.\nThe quit command is used to exit the program."
		"\nquit or q can be used alone or with an optional option."
		"\nThe option can be -i for immediate termination or -d for delayed termination."
		"\nBy default the quit command uses immediate termination.\n";
		show_help_menu_with_text(msg);
	}
	else if( strcmp(quit_type, "-clear") == 0) {
		char *msg = "\nThe clear command.\nThe clear command is used to clear the screen\n";
		show_help_menu_with_text(msg);
	}
	else {
		printf("Unsupported command/mode [%s]\n", quit_type);
		return 0;
	}
}


/*
* Helper function to print a message 
* for a specific command.
*/
void show_help_menu_with_text(char * text) {
	printf("%s\n", text);
}


int clear_screen() {
	system("clear");
}

