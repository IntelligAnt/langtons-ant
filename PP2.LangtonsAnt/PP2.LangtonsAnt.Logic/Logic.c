#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"

Ant *ant_new(Grid *grid, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->p.x = ant->p.y = grid->size / 2;
	ant->dir = dir;
	return ant;
}

void ant_delete(Ant *ant)
{
	free(ant);
}

void ant_move(Ant *ant, Grid *grid, Colors *colors)
{
	int x = ant->p.x, y = ant->p.y;
	if (grid->c[x][y] == COLOR_WHITE) {
		grid->c[x][y] = colors->first;
	}
	int turn = colors->turn[grid->c[x][y]];

	grid->c[x][y] = (unsigned char)colors->next[grid->c[x][y]];

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

	if (ant_out_of_bounds(ant, grid)) {
		expand_grid(grid, ant);
	}
}

bool ant_out_of_bounds(Ant *ant, Grid *grid)
{
	return ant->p.x < 0 || ant->p.x >= grid->size
		|| ant->p.y < 0 || ant->p.y >= grid->size;
}


Colors *init_colors(void)
{
	Colors *new = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_WHITE; ++i) {
		new->turn[i] = 0;
		new->next[i] = COLOR_WHITE;
	}
	new->n = 0;
	new->first = new->last = -1;
	return new;
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
		for (i = 0; i < COLOR_WHITE && colors->next[i] != c; ++i);
		colors->next[i] = colors->next[c];
		if (colors->last == c) {
			colors->last = i;
		}
	}
	colors->next[c] = COLOR_WHITE;
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

bool color_exists(Colors *colors, short c)
{
	return (colors->turn[c]) ? assert(colors->next[c] > -1), 1 : 0;
}

bool enough_colors(Colors *colors)
{
	return colors->n >= 2;
}
