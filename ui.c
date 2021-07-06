/*
 * @file
 *
 * UI implementation: The user interface for the shell
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <locale.h>
#include <signal.h>
#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

#include "history.h"
#include "builtin.h"
#include "logger.h"
#include "ui.h"
#include "util.h"

/* -- Private function forward declarations -- */
static int readline_init(void);
static int key_up(int count, int key);
static int key_down(int count, int key);
char **command_completion(const char *text, int start, int end);
char *command_generator(const char *text, int state);

bool scripting = false;
static char *rline = NULL;
static size_t line_sz = 0;
static char prompt_str1[80];

int cmd = 0;
int currid;
char curr_dir[BUF_SZ];
char home_dir[BUF_SZ];
char host[BUF_SZ];
char user[BUF_SZ];
char *path;
bool exec = true;

// struct command_line * cmds;
char **autocomplete;
int number;

// struct command_line {
//     char **tokens;
//     int tokens_size;
//     bool stdout_pipe;
//     char *stdout_file;
// };

void sigint_donothing(int signo) {
}

void init_ui(void)
{
    LOGP("Initializing UI...\n");
    
    gethostname(host, BUF_SZ);
    strcpy(user, getlogin());
    struct passwd *hwd = getpwuid(getuid());
    strcpy(home_dir, hwd->pw_dir);

    init_builtin();

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    // cmds = malloc(BUF_SZ * sizeof(struct command_line));
    autocomplete = malloc(BUF_SZ * sizeof(char));
    number = 0;
    currid = 0;

    rl_startup_hook = readline_init;
    signal(SIGINT, sigint_donothing);

    path = getenv("PATH");
}

char *prompt_line1() {
    prompt_str1[0] = '\0';

    getcwd(curr_dir, BUF_SZ);
    if (strcmp(home_dir, curr_dir) == 0) {
        strcpy(curr_dir, "~");
    } else if (strstr(curr_dir, home_dir) != NULL) {
        int i;
        bool same = true;
        for (i = 0; i < BUF_SZ; i++) {
            if (home_dir[i] != '\0' && curr_dir[i] != home_dir[i]) {
                same = false;
                break;
            }
        }
        if (same) {
            char tmp[BUF_SZ] = "~";
            int t = 1;
            i = 0;
            while (curr_dir[i] == home_dir[i]) {
                i++;
            }
            while (curr_dir[i] != '\0') {
                tmp[t] = curr_dir[i];
                t++;
                i++;
            }
            strcpy(curr_dir, tmp);
        }
    }

    strcpy(prompt_str1, "[");
    if (exec) {
        strcat(prompt_str1, "🙂");
    } else {
        strcat(prompt_str1, "😭");
    }
    strcat(prompt_str1, "|");
    char num[16];
    sprintf(num, "%d", cmd);
    strcat(prompt_str1, num);
    strcat(prompt_str1, "|");
    strcat(prompt_str1, user);
    strcat(prompt_str1, "@");
    strcat(prompt_str1, host);
    strcat(prompt_str1, " ");
    strcat(prompt_str1, curr_dir);
    strcat(prompt_str1, "]$ ");
    return prompt_str1;
}

char *read_command(bool e)
{
    exec = e;
    if (scripting) {
        ssize_t nread = getline(&rline, &line_sz, stdin);
        if (nread == -1) {
            LOGP("Reached end of input stream.\n");
            close(fileno(stdin));
            return NULL;
        }
        size_t newline = strcspn(rline, "\n");
        rline[newline] = '\0';
        cmd++;
        fflush(stdout);
        return rline;
    } else {
        cmd++;
        return readline(prompt_line1());
    }
}

int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;

    return 0;
}

int key_up(int count, int key)
{
    const char *currcmd = {0};
    if (currid > 0) {
        currcmd = hist_search_cnum(currid);
    }

    if (currcmd != NULL) {
        rl_replace_line(currcmd, 1);
        if (currid-1 != 0) { 
            currid--;
        }
    }
    rl_point = rl_end;
    return 0;
}

int key_down(int count, int key)
{
    const char *currcmd = hist_search_cnum(currid+1);
    if (currcmd != NULL) {
        rl_replace_line(currcmd, 1);
        currid++;
    } 
    // else {
    //     currid--;
    // }
    rl_point = rl_end;
    return 0;
}

char **command_completion(const char *text, int start, int end)
{
    if (strlen(text) == 0) {
        rl_attempted_completion_over = 1;
    } else {
        rl_attempted_completion_over = 0;
    }
    
    return rl_completion_matches(text, command_generator);
}

/**
 * This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL.
 */
char *command_generator(const char *text, int state)
{
    // LOG("State: %d\n", state);
    if (state == 0) {
        char *envs = malloc(sizeof(char) * 256);
        strcpy(envs, path);
        char *dir;

        while ((dir = next_token(&envs, ":"))) {
            DIR *directory;
            if ((directory = opendir(dir)) == NULL) {
                perror("opendir");
                continue;
            }

            struct dirent *entry;
            while ((entry = readdir(directory)) != NULL) {
            	int i = 0;
            	bool match = true;
            	while (text[i] != '\0' && entry->d_name[i] != '\0') {
            		if (text[i] != entry->d_name[i]) {
            			match = false;
            			break;
            		}
            		i++;
            	}

            	if (match) {
            		closedir(directory);
            		char *command = malloc(sizeof(char) * 256);
            		strcpy(command, entry->d_name);
                    // strcpy(autocomplete[number], entry->d_name);
                    // LOG("CMD: %s\n", autocomplete[number]);
                    // number++;
            		return command;
            	}
    	    }
            closedir(directory);
        }

        char *built_in[] = {
            "exit",
            "jobs",
            "history",
            "cd",
        };

        int x;
        for (x = 0; x < 4; x++) {
            bool match = true;
            int i = 0;
            while (text[i] != '\0' && built_in[x][i] != '\0') {
                if (text[i] != built_in[x][i]) {
                    match = false;
                    break;
                }
                i++;
            }

            if (match) {
                char *command = malloc(sizeof(char) * 256);
                strcpy(command, built_in[x]);
                // strcpy(autocomplete[number], built_in[x]);
                // number++;
                return command;
            }
        }
    }
    // LOG("CMD? %s\n", autocomplete[number]);
    // number = 0;
    // while (number < BUF_SZ && autocomplete[number] != NULL) {
    //     number++;
    //     return autocomplete[number];
    // }
    // // TODO: find potential matching completions for 'text.' If you need to
    // // initialize any data structures, state will be set to '0' the first time
    // // this function is called. You will likely need to maintain static/global
    // // variables to track where you are in the search so that you don't start
    // // over from the beginning.
    // closedir(directory);
    // LOGP("No MAtches\n");
    return NULL;
}

void isScripting(int isScripting) {
    if (isScripting == 1) {
        scripting = true;
    } else {
        scripting = false;
    }
}

void execute_cmd(char ** tokens) {
    LOG("Executing %s\n", tokens[0]);
    if (strcmp(tokens[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(tokens[0], "cd") == 0) {
        exit(0);
    }
    if (execvp(tokens[0], tokens) < 0) {
        LOG("%s died\n", tokens[0]);
        exit(1);
    }
}

int runner() {
    hist_init(HIST_SZ);
    bool e = true;

    struct command_line *cmds;
    while (true) {
        char *command = read_command(e);
        e = true;
        if (command == NULL) {
            if (!isatty(STDIN_FILENO)) {
                close(fileno(stdin));
            }
            break;
        }

        cmds = (struct command_line *) malloc(ARG_MAX * sizeof(struct command_line)); // Need to fix this memory leak
        // cmds = (struct command_line *) malloc(5 * sizeof(struct command_line));
        char line[BUF_SZ];
        strcpy(line, command);

        int comm_sz = get_commands(command, cmds);

        if (comm_sz >= 0 && cmds[0].tokens[0] != NULL) {
            if (strcmp(cmds[0].tokens[0], "exit") == 0) {
                break;
            }
            if (strcmp(cmds[0].tokens[0], "cd") == 0) {
                hist_add(line);
                change_dir(cmds[0].tokens_size, cmds[0].tokens);
            } else if (strcmp(cmds[0].tokens[0], "history") == 0) {
                hist_add(line);
                hist_print();
            } else if (strcmp(cmds[0].tokens[0], "jobs") == 0) {
                // Print jobs
            } else {
                if (cmds[0].tokens[0][0] == '!') {
                    if (cmds[0].tokens[0][1] == '!') {
                        int command_number = hist_last_cnum();
                        const char *hist_cmd = hist_search_cnum(command_number);
                        if (hist_cmd == NULL) {
                            LOG("Crash: !%d: event not found\n", command_number);
                            continue;
                        }
                        strcpy(command, hist_cmd);
                        hist_add(hist_cmd);
                        comm_sz = get_commands(command, cmds);
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
                        const char *hist_cmd = hist_search_cnum(command_number);
                        if (hist_cmd == NULL) {
                            LOG("Crash: !%d: event not found\n", command_number);
                            e = false;
                            continue;
                        }
                        strcpy(command, hist_cmd);
                        hist_add(hist_cmd);
                        comm_sz = get_commands(command, cmds);
                    } else {
                        char pref[BUF_SZ];
                        int i = 0;
                        while (cmds[0].tokens[0][i+1] != '\0') {
                            pref[i] = cmds[0].tokens[0][i+1];
                            i++;
                        }
                        pref[i] = '\0';
                        const char *tmp = hist_search_prefix(pref);
                        if (tmp == NULL) {
                            LOG("Crash: %s: event not found\n", command);
                            e = false;
                            continue;
                        }
                        strcpy(command, tmp);
                        hist_add(command);
                        comm_sz = get_commands(command, cmds);
                    }
                } else {
                    hist_add(line);
                }
                
                pid_t child = fork();
                if (child == 0) {
                    execute_pipeline(cmds);
                    if (!isatty(STDIN_FILENO)) {
                        close(fileno(stdin));
                    }
                    return EXIT_FAILURE;
                } else if (child < 0) {
                    perror("fork");
                    e = false;
                } else {
                    int status = 0;
                    //if &, waitpid(-1, status, WNOHANG);
                    waitpid(child, &status, 0);
                    // waitpid(-1, &status, WNOHANG);

                    // LOG("We're the parent, %d\n", status);
                    if (status == 0) {
                        e = true;
                    } else {
                        e = false;
                    }
                }
            }
        }
        currid = hist_last_cnum();
    }

    // hist_destroy();
    // ui_destroy();
    free(cmds);
    return 0;
}

void ui_destroy() {
    hist_destroy();
}