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
#include <conio.h>

/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  4 
#define MAXCMDLINE   64 

/*
 * The run command - submit a job.
 */
int cmd_run(int nargs, char **args);

int list(int nargs, char **args);
/*
 * The quit command.
 */
int cmd_quit(int nargs, char **args);
void clear_screen();
/*
 * Display menu information
 */
void showmenu(const char *name, const char *x[]);

int cmd_helpmenu(int n, char **a);



