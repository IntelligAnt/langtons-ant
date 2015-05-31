#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"

Grid *grid_new(int size)
{
	char **grid = malloc(size * sizeof(char*)), def_color = 15;
	Grid *new = malloc(sizeof(Grid));
	int i, j;
	for (i = 0; i < size; ++i) {
		grid[i] = malloc(size * sizeof(char));
		for (j = 0; j < size; ++j) {
			grid[i][j] = def_color;
		}
	}
	new->g = grid;
	new->size = size;
	return new;
}

Colors *init_colors(void)
{
	Colors *new = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_COUNT - 1; i++) {
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

int new_color(Colors *colors, short c, short turn)
{
	colors->n++;
	if (colors->first == -1) {
		colors->first = c;
	} else {
		colors->next[colors->last] = c;
		colors->next[c] = colors->first;
	}
	colors->last = c;
	colors->turn[c] = turn;
}

int delete_color(Colors *colors, short c)
{
	int i;
	colors->n--;
	colors->turn[c] = 0;
	if (colors->n == 0) {
		colors->first = colors->last = -1;
	} else if (colors->first == c) {
		colors->first = colors->next[colors->last] = colors->next[c];
	} else {
		for (i = 0; i < COLOR_COUNT - 1 && colors->next[i] != c; i++);
		colors->next[i] = colors->next[c];
		if (colors->last == c) {
			colors->last = i;
		}
	}
	colors->next[c] = COLOR_COUNT - 1;
}

int enough_colors(Colors *colors)
{
	return colors->n >= 2;
}

Ant *ant_new(Grid *grid, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->v.x = ant->v.y = grid->size / 2;
	ant->dir = dir;
	return ant;
}

int ant_move(Ant *ant, Grid *grid, Colors *colors)
{
	int x = ant->v.x, y = ant->v.y;
	if (grid->g[x][y] == COLOR_COUNT - 1) {
		grid->g[x][y] = colors->first;
	}
	int turn = colors->turn[grid->g[x][y]];
	
	grid->g[x][y] = colors->next[grid->g[x][y]];

	assert(abs(turn) == 1);

	switch (ant->dir) {
	case UP:
		ant->v.y += turn;
		break;
	case RIGHT:
		ant->v.x += turn;
		break;
	case DOWN:
		ant->v.y -= turn;
		break;
	case LEFT:
		ant->v.x -= turn;
		break;
	}

	ant->dir = (ant->dir + turn + 4) % 4;
}