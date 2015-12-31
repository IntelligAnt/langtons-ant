/**
 * @file io.h
 * Input/output members and function declarations
 * @author IntelligAnt
 */

#ifndef __IO_H__
#define __IO_H__

#include "logic.h"

/*---------------------- Input/output attributes -----------------------*/

/** Maximum filename buffer length on Windows */
#define FILENAME_BUF_LEN   256

/** Total number of fields in a Colors struct */
#define RULES_TOTAL_FIELDS (COLOR_COUNT*2 + 4)


/*----------------------------------------------------------------------*
 *                                 io.c                                 *
 *----------------------------------------------------------------------*/

/**
 * Attemps to read rules from a .lant textfile
 * @param filename Path of file from which to load
 * @return Pointer to a Colors struct if successful; NULL otherwise
 * @see save_rules(char *, Colors *)
 */
Colors* load_rules(char *filename);

/**
 * Attempts to write rules to a .lant textfile
 * @param filename Path of file to which to save
 * @param colors Rules to be written
 * @return Number of read fields if successful; EOF otherwise
 * @see load_rules(char *)
 */
int save_rules(char *filename, Colors *colors);

/**
 * Attemps to read a simulation from a .lant textfile
 * @param filename Path of file from which to load
 * @return Pointer to a Simulation struct if successful; NULL otherwise
 * @see save_simulation(char *, Simulation *)
 */
Simulation *load_simulation(char *filename);

/**
 * Attempts to write a simulation to a .lant textfile
 * @param filename Path of file to which to save
 * @param simulation Simulation to be written
 * @return Number of read fields if successful; EOF otherwise
 * @see load_simulation(char *)
 */
int save_simulation(char *filename, Simulation *simulation);

#endif
