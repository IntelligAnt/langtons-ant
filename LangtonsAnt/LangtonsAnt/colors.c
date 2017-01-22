#include <stdlib.h>
#include "logic.h"

Colors *colors_new(short def)
{
	assert(def >= 0 && def < COLOR_COUNT);
	Colors *colors = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		colors->next[i] = def;
		colors->turn[i] = TURN_NONE;
	}
	colors->n = 0;
	colors->first = colors->last = COLOR_NONE;
	colors->def = def;
	return colors;
}

void colors_delete(Colors *colors)
{
	free(colors);
}

static void update_def(Colors *c)
{
	c->next[c->def] = c->next[c->first];
	c->turn[c->def] = c->turn[c->first];
}

void add_color(Colors *colors, short c, short turn)
{
	if (c < 0 || c >= COLOR_COUNT || c == colors->def) {
		return;
	}
	
	if (colors->first == COLOR_NONE) {
		assert(colors->last == COLOR_NONE);
		colors->first = c;
		colors->last = c;
	}
	colors->next[colors->last] = c;
	colors->next[c] = colors->first;
	colors->last = c;
	colors->turn[c] = turn;

	update_def(colors);
	++colors->n;
}

void remove_color(Colors *colors, short c)
{
	short i;
	if (c < 0 || c >= COLOR_COUNT || c == colors->def || colors->n == 0) {
		return;
	}

	colors->turn[c] = colors->turn[colors->def] = TURN_NONE;
	if (colors->n-- == 1) {
		colors->first = colors->last = COLOR_NONE;
		colors->next[c] = colors->next[colors->def] = colors->def;
		return;
	}

	if (colors->first == c) {
		colors->first = colors->next[colors->last] = colors->next[c];
	}
	for (i = 0; i < COLOR_COUNT; ++i) {
		if (colors->next[i] == c) {
			colors->next[i] = colors->next[c];
			if (colors->turn[i] != TURN_NONE && colors->last == c) {
				colors->last = i;
			}
		}
	}
	update_def(colors);
}

void remove_all_colors(Colors *colors)
{
	short i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		colors->next[i] = colors->def;
		colors->turn[i] = TURN_NONE;
	}
	colors->first = colors->last = COLOR_NONE;
	colors->n = 0;
}

void set_color(Colors *colors, size_t index, short c, short turn)
{
	short prev = colors->last, i = colors->first, j;
	assert(index < colors->n);
	for (; index > 0; --index) {
		prev = i;
		i = colors->next[i];
	}

	colors->next[prev] = c;
	colors->next[c] = colors->next[i];
	colors->turn[c] = turn;
	
	colors->next[i] = c; // Special
	for (j = 0; j < COLOR_COUNT; ++j) {
		if (colors->next[j] == i) {
			colors->next[j] = c;
		}
	}
	colors->turn[i] = TURN_NONE;

	if (i == colors->first) {
		colors->first = c;
	}
	if (i == colors->last) {
		colors->last = c;
	}
	update_def(colors);
}

void set_turn(Colors *colors, size_t index, short turn)
{
	short i = colors->first;
	assert(index < colors->n);
	for (; index > 0; --index) {
		i = colors->next[i];
	}
	colors->turn[i] = turn;
	update_def(colors);
}

short get_color_at(Colors *colors, size_t index)
{
	short i = colors->first;
	assert(index < colors->n);
	for (; index > 0; --index) {
		i = colors->next[i];
	}
	return i;
}

bool color_exists(Colors *colors, short c)
{
	return colors->turn[c] != TURN_NONE;
}

bool is_color_special(Colors *colors, short c)
{
	return colors->next[c] != colors->def && colors->turn[c] == TURN_NONE;
}

bool is_colors_empty(Colors *colors)
{
	return colors->n == 0;
}

bool has_enough_colors(Colors *colors)
{
	return colors->n >= 2;
}
