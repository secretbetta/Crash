/**
 * @file
 *
 * Functions that implement the built-in shell commands such as 'cd', 'exit', etc.
 *
 */

#include <stdbool.h>
#include <sys/types.h>

#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#ifndef BUF_SZ
#define BUF_SZ 4096
#endif

#ifndef JOB_SZ
#define JOB_SZ 10
#endif

#define ARG_MAX 4096

/**
 * Command line elements
 */
struct command_line {
	char **tokens;
	int tokens_size;
	bool stdout_pipe;
	char *stdout_file;
	bool bg;
};

/**
 * Background process
 */
struct b_process {
	pid_t pid;
	char *command; 
};

/**
 * Executes piping
 */
int execute_pipeline(struct command_line *cmds);
/** 
 * Initializes builtin
 */
void init_builtin(void);
/** 
 * Handles built in commands
 */
int handle_builtin(struct command_line *cmds, int argc, char *line);
/** 
 * Changes directory of shell
 */
int change_dir(int argc, char *argv[]);
/**
 * Gets command tokens and stuff
 */
int run(char *line);
/**
 * Gets commands and tokenizes them into command_line
 */
int get_commands(char *command, struct command_line *cmds);
/**
 * Frees all allocated variables in builtin
 */
int free_builtin(void);

#endif