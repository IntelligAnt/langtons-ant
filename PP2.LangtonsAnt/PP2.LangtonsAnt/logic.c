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

static void ant_dir_turn(Ant *ant, int turn)
{
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

static void update_bounding_box(Vector2i pos, Vector2i *tl, Vector2i *br)
{
	tl->y = min(tl->y, pos.y);
	tl->x = min(tl->x, pos.x);
	br->y = max(br->y, pos.y);
	br->x = max(br->x, pos.x);
}

static bool is_grid_usage_low(Grid *grid)
{
	int b = (grid->bottom_right.y - grid->top_left.y + 1)*(grid->bottom_right.x - grid->top_left.x + 1);
	return (double)grid->used / b < GRID_USAGE_THRESHOLD;
}

static void ant_move_n(Ant *ant, Grid *grid, Colors *colors)
{
	int y = ant->pos.y, x = ant->pos.x, turn;
	bool is_def = grid->c[y][x] == colors->def;
	if (is_def) {
		grid->c[y][x] = (unsigned char)colors->first;
		grid->used++;
		update_bounding_box(ant->pos, &grid->top_left, &grid->bottom_right);
	}
	turn = colors->turn[grid->c[y][x]];
	assert(abs(turn) == 1);
	grid->c[y][x] = (unsigned char)colors->next[grid->c[y][x]];
	ant_dir_turn(ant, turn); 
	if (is_def && IS_GRID_LARGE(grid) && is_grid_usage_low(grid)){
		grid_make_sparse(grid);
	}
}

static void ant_move_s(Ant *ant, Grid *grid, Colors *colors)
{
	int y = ant->pos.y, x = ant->pos.x, turn;
	Cell **t = grid->csr+y;

	while (*t && CELL_GET_COLUMN(*t) < x) {
		t = &((*t)->next);
	}
	if (!*t || CELL_GET_COLUMN(*t) != x) {
		new_cell(t, x, (unsigned char)colors->first);
	}

	turn = colors->turn[CELL_GET_COLOR(*t)];
	assert(abs(turn) == 1);
	CELL_SET_COLOR(*t, (unsigned char)colors->next[CELL_GET_COLOR(*t)]);
	ant_dir_turn(ant, turn);
}

bool ant_move(Ant *ant, Grid *grid, Colors *colors) // TODO have ant_move return move direction (either relative or absolute)
{
	if (is_grid_sparse(grid)) {
		ant_move_s(ant, grid, colors);
	} else {
		ant_move_n(ant, grid, colors);
	}
	return !is_ant_out_of_bounds(ant, grid);
}

bool is_ant_out_of_bounds(Ant *ant, Grid *grid)
{
	return ant->pos.y < 0 || ant->pos.y >= grid->size
		|| ant->pos.x < 0 || ant->pos.x >= grid->size;
}

/* Color functions */

Colors *colors_new(short def)
{
	assert(def >= 0 && def < COLOR_COUNT);
	Colors *colors = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_COUNT; ++i) {
		colors->next[i] = def;
		colors->turn[i] = 0;
	}
	colors->n = 0;
	colors->first = colors->last = -1;
	colors->def = def;
	return colors;
}

void colors_delete(Colors *colors)
{
	free(colors);
}

void add_color(Colors *colors, short c, short turn)
{
	if (c < 0 || c >= COLOR_COUNT) {
		return;
	}
	if (colors->first == -1) {
		colors->first = c;
	} else {
		colors->next[colors->last] = c;
		colors->next[c] = colors->first;
	}
	colors->last = c;
	colors->turn[c] = turn;
	++colors->n;
}

void remove_color(Colors *colors, short c)
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
	return c == colors->def || colors->turn[c];
}

bool has_enough_colors(Colors *colors)
{
	return colors->n >= 2;
}
