/**
 * @file
 *
 * Functions that implement the built-in shell commands such as 'cd', 'exit', etc.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#include "history.h"
#include "logger.h"
#include "builtin.h"
#include "util.h"

char curr_dir[BUF_SZ];
char home_dir[BUF_SZ];
int home_size;
int curr_size;
struct b_process *bp;

/**
 * Sets current directory to home directory
 */
void set_home(void) {
	strcpy(curr_dir, home_dir);
	chdir(curr_dir);
}

/** 
 * Initializes built in
 */
void init_builtin(void) {
    bp = malloc(sizeof(struct b_process) * JOB_SZ);
	struct passwd *hwd = getpwuid(getuid());
    strcpy(home_dir, hwd->pw_dir);
}

int execute_pipeline(struct command_line *cmds) {
    if (cmds[0].tokens_size == -1) {
        return -1;
    }

    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 0;
    }

    if (!cmds[0].stdout_pipe) {
        if (cmds[0].stdout_file != NULL) {
            fd[0] = open(cmds[0].stdout_file, O_CREAT |  O_WRONLY | O_TRUNC, 0666);
            dup2(fd[0], STDOUT_FILENO);
        }
        LOG("Token running: \'%s\'\n", cmds[0].tokens[0]);
        int x = execvp(cmds[0].tokens[0], cmds[0].tokens);
        LOG("Execvp returned: %d\n", x);
        perror("Execvp");
        return 0;   
    }

    pid_t pid = fork();

    if (pid == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execvp(cmds[0].tokens[0], cmds[0].tokens);
    } else if (pid < 0) {
        perror("fork");
        return -1;   
    } else {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        execute_pipeline(&cmds[1]);
    }
    return 0;
}

int handle_builtin(struct command_line *cmds, int argc, char *line) {
    if (strcmp(cmds[0].tokens[0], "exit") == 0) {
        return 0;
    }
    if (strcmp(cmds[0].tokens[0], "cd") == 0) {
    	hist_add(line);
        change_dir(cmds[0].tokens_size, cmds[0].tokens);
    } else if (strcmp(cmds[0].tokens[0], "history") == 0) {
    	hist_add(line);
        hist_print();
    } else {
        if (cmds[0].tokens[0][0] == '!') {
            if (cmds[0].tokens[0][1] == '!') {
                int command_number = hist_last_cnum();
                LOG("Command number: %d\n", command_number);
                const char *hist_cmd = hist_search_cnum(command_number);
                if (hist_cmd == NULL) {
                    printf("Crash: !%d: event not found\n", command_number);
                    return -1;
                }
                strcpy(line, hist_cmd);
                hist_add(hist_cmd);
                get_commands(line, cmds);
            } else if (isdigit(cmds[0].tokens[0][1])) {
                char tmp[256];
                int i;
                for (i = 1; i < 256; i++) {
                    if (cmds[0].tokens[0][i] == '\n') {
                        tmp[i-1] = '\0';
                        break;
                    }
                    tmp[i-1] = cmds[0].tokens[0][i];
                }

                int command_number = atoi(tmp);
                LOG("Command number: %d\n", command_number);
                const char *hist_cmd = hist_search_cnum(command_number);
                if (hist_cmd == NULL) {
                    printf("Crash: !%d: event not found\n", command_number);
                    return -1;
                }
                strcpy(line, hist_cmd);
                hist_add(hist_cmd);
                get_commands(line, cmds);
            } else {
                char pref[BUF_SZ];
                int i = 0;
                while (cmds[0].tokens[0][i+1] != '\0') {
                    pref[i] = cmds[0].tokens[0][i+1];
                    i++;
                }
                const char *tmp = hist_search_prefix(pref);
                if (tmp == NULL) {
                    printf("Crash: %s: event not found\n", line);
                    return -1;
                }
                strcpy(line, tmp);
                hist_add(line);
                get_commands(line, cmds);
            }
        } else {
            hist_add(line);
        }
    }
    return 0;
}

int get_commands(char *command, struct command_line *cmds) {
	int comm_sz = -1;
	char *next_tok = command;
	char *curr_tok;
	char **curr_tokens;
	struct command_line *cmd;

	bool multi = true;

	while ((curr_tok = next_token(&next_tok, " \'\"\t\n")) != NULL) {
	    if (strcmp(curr_tok, "|") == 0 && !multi) {
	    	multi = true;
	        cmd->stdout_pipe = true;
	        continue;
	    }
        bool bg = false;
        if (strcmp(curr_tok, "&") == 0) {
           bg = true;
           LOG("Background process found: %d\n", bg);
        }

	    if (curr_tok[0] == '#') {
	        break;
	    }

	    if (strcmp(curr_tok, ">") == 0 && !multi) {
	        if (comm_sz != -1) {
	            curr_tok = next_token(&next_tok, " \'\"\t\n");
	            if (curr_tok != NULL) {
	                cmd->stdout_file = malloc(BUF_SZ);
	                strcpy(cmd->stdout_file, curr_tok);
	            }
	            break;
	        }
	    }

	    if (multi) {
	        if (comm_sz != -1) {
	            curr_tokens[cmd->tokens_size++] = (char *) NULL;
	        }

	        comm_sz += 1;
	        multi = false;

	        cmd = &cmds[comm_sz];

	        cmd->stdout_pipe = false;
	        cmd->stdout_file = NULL;
            cmd->bg = bg;

	        cmd->tokens = (char **) malloc(ARG_MAX * sizeof(char *)); // Memory leak
	        curr_tokens = cmd->tokens;
	        cmd->tokens_size = 0;
	    }

	    curr_tokens[cmd->tokens_size] = (char *) malloc(BUF_SZ); // memory leak
	    strcpy(curr_tokens[cmd->tokens_size++], curr_tok);
	}
	return comm_sz;
}

int change_dir(int argc, char *argv[]) {
	LOGP("changing dir\n");
    if (argc == 1) {
        LOGP("Going to home via cd\n");
        chdir(home_dir);
        return 1;
    } else {
        if (strcmp(argv[1], "~") == 0) {
            LOGP("Going to home via ~\n");
            chdir(home_dir);
            return 1;
        } else if (chdir(argv[1])) {
            LOG("cd to DIR: %s\n", argv[1]);
            return 1;
        } else {
            getcwd(curr_dir, BUF_SZ);
        }
    }
    return 0;
}

int free_builtin(void) {
    free(bp);
    hist_destroy();
    return 1;
}