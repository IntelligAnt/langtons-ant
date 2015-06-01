#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"

Grid *grid_new(unsigned size)
{
	Grid *grid = malloc(sizeof(Grid));
	unsigned char **c = malloc(size * sizeof(unsigned char*));
	short def_color = COLOR_WHITE;
	unsigned i, j;
	for (i = 0; i < size; ++i) {
		c[i] = malloc(size * sizeof(unsigned char));
		for (j = 0; j < size; ++j) {
			c[i][j] = def_color;
		}
	}
	grid->c = c;
	grid->size = size;
	return grid;
}

Colors *init_colors(void)
{
	Colors *new = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_COUNT-1; ++i) {
		new->turn[i] = 0;
		new->next[i] = COLOR_COUNT - 1;
	}
	new->n = 0;
	new->first = new->last = -1;
	return new;
}

int color_exists(Colors *colors, short c)
{
	return colors->turn[c] != 0;
}

void new_color(Colors *colors, short c, short turn)
{
	++colors->n;
	if (colors->first == -1) {
		colors->first = c;
	} else {
		colors->next[colors->last] = c;
		colors->next[c] = colors->first;
	}
	colors->last = c;
	colors->turn[c] = turn;
}

void delete_color(Colors *colors, short c)
{
	short i;
	--colors->n;
	colors->turn[c] = 0;
	if (colors->n == 0) {
		colors->first = colors->last = -1;
	} else if (colors->first == c) {
		colors->first = colors->next[colors->last] = colors->next[c];
	} else {
		for (i = 0; i < COLOR_COUNT-1 && colors->next[i] != c; ++i);
		colors->next[i] = colors->next[c];
		if (colors->last == c) {
			colors->last = i;
		}
	}
	colors->next[c] = COLOR_COUNT - 1;
}

void set_color(Colors *colors, short index, short c, short turn)
{
	short i = colors->first, prev = colors->last;
	assert(index >= 0 && index < colors->n);
	assert(!color_exists(colors, c));
	while (index--) {
		prev = i;
		i = colors->next[i];
	}
	colors->next[prev] = c;
	colors->turn[i] = turn;
}

int enough_colors(Colors *colors)
{
	return colors->n >= 2;
}

Ant *ant_new(Grid *grid, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->p.x = ant->p.y = grid->size / 2;
	ant->dir = dir;
	return ant;
}

void ant_move(Ant *ant, Grid *grid, Colors *colors)
{
	int x = ant->p.x, y = ant->p.y;
	if (grid->c[x][y] == COLOR_COUNT-1) {
		grid->c[x][y] = colors->first;
	}
	int turn = colors->turn[grid->c[x][y]];
	
	grid->c[x][y] = (unsigned char) colors->next[grid->c[x][y]];

	assert(abs(turn) == 1);

	switch (ant->dir) {
	case UP:
		ant->p.y += turn;
		break;
	case RIGHT:
		ant->p.x += turn;
		break;
	case DOWN:
		ant->p.y -= turn;
		break;
	case LEFT:
		ant->p.x -= turn;
		break;
	}

	ant->dir = (ant->dir + turn + 4) % 4;
}