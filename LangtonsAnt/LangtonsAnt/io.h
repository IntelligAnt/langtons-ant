/**
 * @file io.h
 * File containing all input/output members and functions
 * @author IntelligAnt
 */

#ifndef __IO_H__
#define __IO_H__

#include "logic.h"

/** Maximum filename buffer length on Windows */
#define FILENAME_BUF_LEN   256

/** Total number of fields in a Colors struct */
#define RULES_TOTAL_FIELDS (COLOR_COUNT*2 + 4)

/**
 * Attemps to read rules from a .lant textfile
 * @param filename Path of file from which to load
 * @return Pointer to a Colors struct if successful; NULL otherwise
 * @see save_rules(char *, Colors *)
 */
Colors* load_rules(char *filename);

/**
 * Attempts to write rules to a .lant textfile
 * @param filename Path of file from which to load
 * @param colors Rules to be written
 * @return Number of read fields if successful; EOF otherwise
 * @see load_rules(char *)
 */
int save_rules(char *filename, Colors *colors);

#endif
