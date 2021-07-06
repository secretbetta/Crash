/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 */

#include <stdbool.h>

#ifndef _UI_H_
#define _UI_H_

#define BUF_SZ 4096

#ifndef HIST_SZ
#define HIST_SZ 100
#endif

/** 
 * Initializes UI
 */
void init_ui(void);
/** 
 * Prompt line for shell
 */
char *prompt_line1(void);
/** 
 * Reads command and execs it
 */
char *read_command(bool e);
/**
 * Changes scripting mode on or off
 */
void isScripting(int isScripting);
/**
 * Runs the whole shell
 */
int runner();
/**
 * Destroys all inits in ui
 */
void ui_destroy();
#endif
