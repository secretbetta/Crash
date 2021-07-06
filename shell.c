/*
 * @file
 *
 * Shell implementation: A shell program that runs on linux
 */

#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "builtin.h"

/**
 * Main method
 * Runs the whole shell
 */
int main(void)
{
    init_ui();

    if (isatty(STDIN_FILENO)) {
        LOGP("stdin is a TTY; entering interactive mode\n");
        isScripting(0);
    } else {
    	isScripting(1);
        LOGP("data piped in on stdin; entering script mode\n");
    }
    init_builtin();

    runner();

    return 0;
}
