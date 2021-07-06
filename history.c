/*
 * @file
 *
 * History implementation: Functions for the 
 * history of commands of the shell program
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "history.h"
#include "logger.h"

/** 
 * History entry
 * cmd_id = command id
 * run_time = runtime of command
 * line = command line
 */
struct history_entry {
    unsigned long cmd_id;
    double run_time;
    char *line;
};

struct history_entry *hist_list = {0};
int counter = 0;
int curr = 0;
int id = 1;
int lim = 0;
int circ = 0;

void hist_init(unsigned int limit)
{
    lim = limit;
    hist_list = malloc(sizeof(struct history_entry) * limit);
}

void hist_destroy(void)
{
    int i;
    for (i = 0; i < lim; i++) {
        if (hist_list[i].line != NULL) {
            free(hist_list[i].line);
        }
    }
}

void hist_add(const char *cmd)
{
    struct history_entry * command = &hist_list[counter%lim];
    command->cmd_id = id++;
    // command->line = malloc(sizeof(char));
    // strcpy(command->line, cmd);
    // char tmp[256];
    // strcpy(tmp, cmd);
    command->line = strdup(cmd); // CAN I FIX THIS?!?!? memory leak

    counter++;
    if (counter > lim) {
        circ++;
    }
}

void hist_print(void)
{
    int i = circ;
    while (i != counter) {
        printf(" %lu %s\n", hist_list[i%lim].cmd_id, hist_list[i%lim].line);
        i++;
    }
}

const char *hist_search_prefix(char *prefix)
{
    bool exact = true;
    int curr_id = id-1;
    const char *tmp;
    while ((tmp=hist_search_cnum(curr_id)) != NULL) {
        int i = 0;
        while (prefix[i] != '\0' && prefix[i] != ' ' && tmp[i] != '\0') {
            if (prefix[i] != tmp[i]) {
                exact = false;
                break;
            }
            i++;
        }
        if (exact) {
            return hist_search_cnum(curr_id);
        }
        exact = true;
        curr_id--;
    }
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    if (command_number > counter || command_number < 0 || command_number < id-lim) {
        return NULL;
    }
    return hist_list[(command_number-1)%lim].line;
}

unsigned int hist_last_cnum(void)
{
    curr = id;
    return curr-1;
}
