#include <stdio.h>
#include <stdlib.h>

#include "io.h"

Colors* load_rules(char *filename)
{
    FILE *in;
    Colors *colors;
    char *buff;
    int t, s, n, w;
	in = fopen(filename, "r");
    fscanf(in, "%s", buff);
    short def;
    def = atoi(buff);
    colors = colors_new(def);
    t = 0;
    w = 1;
    short r = 0;
    short k = 0;
    while((n != 0) || (w == 1)) {
        if ((n == 0) && (t == 2)) {
            w = 0;
        }
        fscanf(in, "%s", buff);
        if (t == 1) {
            s = atoi(buff);
            r = s;
            t = t + 1;
        }
        else
            if (t == 2) {
            int y = atoi(buff);
            k = y;
            if (r == 0) {
                r = -1;
            }
            add_color(colors, k, r);
            t = 0;
        }
        else
            if (t == 0) {
            n = atoi(buff);
            t = t + 1;
        }
    }
    fclose(in);
    return colors;
};

void save_rule(Colors *colors)
{
    FILE *out;
    out = fopen("rules_out.txt", "w");
    int q, n, w;
    char p;
    n = 0;
    q = colors->next[colors->first];
    p = colors->turn[colors->first];
    w = 1;
    int i;
    while ((q != colors->next[colors->last]) || (w == 1)) {
            w = 0;
            q = colors->next[q];
            p = colors->turn[q];
            n = n + 1;
        }
    q = colors->first;
    p = colors->turn[colors->first];
    w = 1;
    fprintf(out, "%d\n", colors->def);
    while ((q != colors->next[colors->last]) || (w == 1)) {
            w = 0;
            if (p == -1)
                p = 0;
            fprintf(out, "%d %d %d\n", n, p, q);
            n = n - 1;
            q = colors->next[q];
            p = colors->turn[q];
        }
    fclose(out);
}
