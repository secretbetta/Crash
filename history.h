/**
 * @file
 *
 * Contains shell history data structures and retrieval functions.
 */

#ifndef _HISTORY_H_
#define _HISTORY_H_

/** 
 * Initializes history
 */
void hist_init(unsigned int);
/** 
 * Free all history members
 */
void hist_destroy(void);
/** 
 * Adds to history
 */
void hist_add(const char *);
/** 
 * Prints history
 */
void hist_print(void);
/** 
 * Searches for command in history using prefix
 */
const char *hist_search_prefix(char *);
/** 
 * Search for command in history via ID
 */
const char *hist_search_cnum(int);
/** 
 * Get the last command in history
 */
unsigned int hist_last_cnum(void);

#endif
