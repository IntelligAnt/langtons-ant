/**
* @file io.h
* @author IntelligAnt
* @brief File containing all input/output functions
*/

#ifndef __IO_H__
#define __IO_H__

#include "logic.h"

/** Maximum filename buffer length on Windows */
#define FILENAME_BUF_LEN   256

/** Total number of fields in a Colors struct */
#define RULES_TOTAL_FIELDS COLOR_COUNT*2+4

/**
* @brief Attemps to read rules from a .lant textfile
* @param filename Path of file from which to load
* @return Pointer to Colors struct if successful; NULL otherwise
*/
Colors* load_rules(char *filename);

/**
* @brief Attempts to write rules to a .lant textfile
* @param filenama Path of file from which to load
* @param colors Rules to be written
* @return Number of read fields if successful; EOF otherwise
*/
int save_rules(char *filename, Colors* colors);

#endif
