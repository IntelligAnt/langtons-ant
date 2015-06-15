#ifndef __IO_H__
#define __IO_H__

#include "logic.h"

Colors* load_rules(char name[255]);
void save_rule(Colors* rule_set);

#endif // __IO_H__
