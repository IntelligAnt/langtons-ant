#include <stdio.h>
#include <string.h>

#include "rules.h"

struct rule* load_rules(char name[256])
{
    FILE *in, *out;
    struct rule *rule_set;
    rule_set = (struct rule*) malloc(sizeof(struct rule));
    rule_set->next = (struct rule*) malloc(sizeof(struct rule));
    rule_set->next->next = NULL;
    char buff[256];
    int t, s, n, w;
    in = fopen(name,"r");
    fscanf(in, "%s", buff);
    n = atoi(buff);
    struct rule *temp;
    temp = rule_set->next;
    temp->col = '5';
    temp->dir = 0;
    t = 1;
    w = 1;
    while(( n != 0 ) || ( w == 1 ))
    {
        if ((n == 0) && (t == 2))
            w = 0;
        fscanf(in, "%s", buff);
        if ( t == 1 )
        {
            s = atoi(buff);
            temp->dir = s;
            t = t + 1;
        }
        else if ( t == 2)
        {
            temp->col = buff[0];
            if (w == 1)
            {
                temp->next = (struct rule *) malloc(sizeof(struct rule));
                temp = temp->next;
            }
            temp->next = NULL;
            t = 0;
        }
        else
            if ( t == 0)
        {
            n = atoi(buff);
            t = t + 1;
        }
    }
    rule_set = rule_set->next;
    temp->next = rule_set;
    fclose(in);
    return rule_set;
};

void save_rule(struct rule *rule_set)
{
    FILE *out;
    struct rule* temp;
    out = fopen("rules_out.txt", "w");
    temp = rule_set;
    int q, n, w;
    char p;
    n = -1;
    q = temp->dir;
    p = temp->col;
    w = 1;
    while ((q != temp->dir) || (p != temp->col) || (w == 1))
        {
            w = 0;
            temp = temp->next;
            n = n + 1;
        }
    q = temp->dir;
    p = temp->col;
    w = 1;
    while ((q != temp->dir) || (p != temp->col) || (w == 1))
        {
            w = 0;
            fprintf(out, "%d %d %c\n", n, temp->dir, temp->col);
            n = n - 1;
            temp = temp->next;
        }
    fclose(out);
}
