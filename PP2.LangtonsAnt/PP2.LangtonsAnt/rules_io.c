#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

Colors *load_rules(char *filename)
{
	Colors *colors;
	FILE *input;
	int e, i;
	short def, c;
	
	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	e = fscanf(input, "%hd\n", &def);
	if (def < 0 || def >= COLOR_COUNT) {
		return NULL;
	}
	colors = colors_new(def);
	colors->def = def;

	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next+c);
	}
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->turn+c);
	}
	e += fscanf(input, "%hd %hd\n", &colors->first, &colors->last);
	e += fscanf(input, "%hd", &colors->n);

	if (e < RULES_TOTAL_FIELDS) {
		colors_delete(colors);
		return NULL;
	}
	return colors;
}

int save_rules(char *filename, Colors* colors)
{
	FILE *output;
	int e;
	short c;

	if (!(output = fopen(filename, "w"))) {
		return EOF;
	}

	e = fprintf(output, "%hd\n", colors->def);
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next[c]);
	}
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->turn[c]);
	}
	e += fprintf(output, "%hd %hd\n", colors->first, colors->last);
	e += fprintf(output, "%hd", colors->n);

	if (fclose(output) == EOF || e < RULES_TOTAL_FIELDS) {
		e = EOF;
	}
	return e;
}
