////////////////////////////////////////////////////////////////////////////////
// File          : shellex.c
// Description   : This file provides functionality of a simple shell.
//
// Author name   : Meagan Nguyen
// Last modified : July 06, 2020

/* $begin shellmain */
#include "csapp.h"

#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
int cd();
int getPID();
int getPPID();
int exitmode();
void helpMenu();
void sigintHandler();

// Description : Main source code of project
// Inputs      : argc - number of arguments
// 	         argv - list of arguments  
int main(int argc, char *argv[]) {
    char cmdline[MAXLINE]; /* Command line */
    printf("Meagan's shell: launched!\n");
    signal(SIGINT, sigintHandler);
    while (1) {
	/* Read */
	if (argc > 1) {
	    printf("%s>", argv[2]);
	}
        else
	    printf("my257sh> "); 
	Fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);
        /* Evaluate */
        eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) {
    char *argv[MAXARGS]; /* Argument list execv() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { 
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
	/* Parent waits for foreground job to terminate */
	if (!bg) {
	    int status;
	    if (waitpid(pid, &status, 0) < 0)
		unix_error("waitfg: waitpid error");
	}
	else
	    printf("%d %s", pid, cmdline);
	    int status;
            	if (WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    printf("Child exit status: %d\n", exit_status);
                }
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) {
    if (!strcmp(argv[0], "help")){
	helpMenu();
	return 1;
    }
    if (!strcmp(argv[0], "pid")){
	getPID();
	return 1;
    }
    if (!strcmp(argv[0], "ppid")){
	getPPID();
	return 1;
    } 
    if (!strcmp(argv[0], "cd")) {
	if (argv[1] == NULL)
	    cd();
	else
	    chdir(argv[1]);
        return 1;
    }
    if (!strcmp(argv[0], "exit")) {
	exitmode();
	return 1;
    }
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) {
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	argv[--argc] = NULL;

    return bg;
}
/* $end parseline */

// Description : Prints the current working directory.
// Outputs     : 0 if done successfully
int cd() {
    char cwd[256];
    printf("%s\n", getcwd(cwd, 256)); 
    return 0;
}

// Description : Prints the process id of the shell.
// Outputs     : 0 if successful
int getPID() {
    pid_t pid;
    pid = getpid();
    printf("%d\n", pid);
    return 0;
}

// Description : Prints the parent process id of the shell.
// Outputs     : 0 if successful
int getPPID() {
    pid_t ppid;
    ppid = getppid();
    printf("%d\n", ppid);
    return 0;
}

// Description : Prints developer name of shell and list of builtin commands.
void helpMenu() {
    puts("-------------------------------------\n"
	"MEAGAN'S SHELL HELP CENTER\n"
	"Built-in commands supported:\n"
	"> cd\n"
	"> pid\n"
	"> ppid\n"
	"> ls\n"
	"> exit\n"
	"For nonbuilt-in commands, please refer to man :)\n"
        "-------------------------------------");
    return;
}

// Description  : Handles signal for SIGINT.
// Inputs       : sig_num - the signal
void sigintHandler(int sig_num) {
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}

// Description  : Exits the shell using raise().
// Outputs      : 0 if successful
int exitmode() {
    raise(SIGTERM);
    return 0;
}
