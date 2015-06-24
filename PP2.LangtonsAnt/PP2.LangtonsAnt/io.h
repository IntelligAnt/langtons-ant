#ifndef __IO_H__
#define __IO_H__

#include "logic.h"

#define FILENAME_BUF_LEN   256
#define RULES_TOTAL_FIELDS COLOR_COUNT*2+4

Colors* load_rules(char *filename);
int save_rules(char *filename, Colors* colors);

#endif
