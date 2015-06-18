#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"

/* Ant functions */

Ant *ant_new(Grid *grid, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->pos.y = ant->pos.x = grid->size / 2;
	ant->dir = dir;
	return ant;
}

void ant_delete(Ant *ant)
{
	free(ant);
}

static void ant_dir_turn(Ant *ant, int turn){
	switch (ant->dir) {
	case UP:
		ant->pos.x += turn;
		break;
	case RIGHT:
		ant->pos.y += turn;
		break;
	case DOWN:
		ant->pos.x -= turn;
		break;
	case LEFT:
		ant->pos.y -= turn;
		break;
	}
	ant->dir = (ant->dir + turn + 4) % 4;
}

bool ant_move(Ant *ant, Grid *grid, Colors *colors)
{
	int y = ant->pos.y, x = ant->pos.x, turn;
	if (grid->c[y][x] == colors->def) {
		grid->c[y][x] = colors->first;
	}
	turn = colors->turn[grid->c[y][x]];
	assert(abs(turn) == 1);
	grid->c[y][x] = (unsigned char)colors->next[grid->c[y][x]];
	ant_dir_turn(ant, turn);
	return !ant_out_of_bounds(ant, grid);
}

bool ant_out_of_bounds(Ant *ant, Grid *grid)
{
	return ant->pos.y < 0 || ant->pos.y >= grid->size
		|| ant->pos.x < 0 || ant->pos.x >= grid->size;
}

bool ant_out_of_bounds_s(Ant *ant, Grid_s *grid)
{
	return ant->pos.y < 0 || ant->pos.y >= grid->size
		|| ant->pos.x < 0 || ant->pos.x >= grid->size;
}

bool ant_move_s(Ant *ant, Grid_s *grid, Colors *colors)
{
	int y = ant->pos.y, x = ant->pos.x, turn;
	Element *t = grid->rows[y];

	while (t && t->column < x){
		t = t->next;
	}
	if (t && t->column > x) {
		t = t->prev;
	}
	if (!t || t->column < x){
		new_element(t, x, (unsigned char)colors->first);
		if (t->column != x) t = t->next;
	}

	turn = colors->turn[t->c];
	assert(abs(turn) == 1);
	t->c = (unsigned char)colors->next[t->c];
	ant_dir_turn(ant, turn);
	return !ant_out_of_bounds_s(ant, grid);
}

/* Color functions */

Colors *init_colors(short def)
{
	assert(def >= 0 && def < COLOR_COUNT);
	Colors *colors = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		if (i == def) {
			continue;
		}
		colors->turn[i] = 0;
		colors->next[i] = def;
	}
	colors->n = 0;
	colors->first = colors->last = -1;
	colors->def = def;
	return colors;
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
	assert(c != colors->def);
	--colors->n;
	colors->turn[c] = 0;
	if (colors->n == 0) {
		colors->first = colors->last = -1;
	} else if (colors->first == c) {
		colors->first = colors->next[colors->last] = colors->next[c];
	} else {
		for (i = 0; i < COLOR_COUNT && colors->next[i] != c; ++i);
		colors->next[i] = colors->next[c];
		if (colors->last == c) {
			colors->last = i;
		}
	}
	colors->next[c] = colors->def;
}

void set_color(Colors *colors, short index, short c, short turn)
{
	short i = colors->first, prev = colors->last;
	assert(index >= 0 && index < colors->n);
	while (index--) {
		prev = i;
		i = colors->next[i];
	}
	colors->next[prev] = c;
	colors->turn[i] = turn;
}

bool color_exists(Colors *colors, short c)
{
	return colors->turn[c] ? assert(colors->next[c] != colors->def), 1 : 0;
}

bool enough_colors(Colors *colors)
{
	return colors->n >= 2;
}
