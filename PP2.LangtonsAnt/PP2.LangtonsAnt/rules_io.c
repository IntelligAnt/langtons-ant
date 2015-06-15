#include <stdio.h>
#include <stdlib.h>

#include "io.h"

Colors* load_rules(char *name)
{
    FILE *in;
    Colors *rule_set;
    char *buff;
    int t, s, n, w;
    in = fopen(name,"r");
    fscanf(in, "%s", buff);
    short def;
    def = atoi(buff);
    rule_set = init_colors(def);
    rule_set->def = def;
    t = 0;
    w = 1;
    short r = 0;
    short k = 0;
    while(( n != 0 ) || ( w == 1 )){
        if ((n == 0) && (t == 2)){
            w = 0;
        }
        fscanf(in, "%s", buff);
        if ( t == 1 ){
            s = atoi(buff);
            r = s;
            t = t + 1;
        }
        else
            if ( t == 2){
            int y = atoi(buff);
            k = y;
            if ( r == 0){
                r = -1;
            }
            new_color(rule_set, k, r);
            t = 0;
        }
        else
            if ( t == 0){
            n = atoi(buff);
            t = t + 1;
        }
    }
    fclose(in);
    return rule_set;
};

void save_rule(Colors *rule_set)
{
    FILE *out;
    out = fopen("rules_out.txt", "w");
    int q, n, w;
    char p;
    n = 0;
    q = rule_set->next[rule_set->first];
    p = rule_set->turn[rule_set->first];
    w = 1;
    int i;
    while ((q != rule_set->next[rule_set->last]) || (w == 1)){
            w = 0;
            q = rule_set->next[q];
            p = rule_set->turn[q];
            n = n + 1;
        }
    q = rule_set->first;
    p = rule_set->turn[rule_set->first];
    w = 1;
    fprintf(out, "%d\n", rule_set->def);
    while ((q != rule_set->next[rule_set->last]) || (w == 1)){
            w = 0;
            if (p == -1)
                p = 0;
            fprintf(out, "%d %d %d\n", n, p, q);
            n = n - 1;
            q = rule_set->next[q];
            p = rule_set->turn[q];
        }
    fclose(out);
}
