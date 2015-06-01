#ifndef RULES_H_INCLUDED
#define RULES_H_INCLUDED

struct rule
{
    int dir;
    char col;
    struct rule *next;
};

int to_int(char buff[255]);
struct rule* load_rules(char name[255]);
void save_rule(struct rule* rule_set);

#endif // RULES_H_INCLUDED
