#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"
#include "graphics.h"

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

void grid_delete(Grid *grid)
{
	unsigned i;
	for (i = 0; i < grid->size; ++i) {
		free(grid->c[i]);
	}
	free(grid->c);
	free(grid);
}

static void transfer_ant(Ant *ant, unsigned old_size)
{
	ant->pos.y += old_size;
	ant->pos.x += old_size;
}

static bool is_in_old_matrix(int y, int x, unsigned old_size)
{
	return y >= old_size && y < 2*old_size && x >= old_size && x < 2*old_size;
}

void expand_grid(Grid *grid, Ant *ant)
{
	unsigned old = grid->size, size = GRID_MUL*old, i, j;
	unsigned char **c = malloc(size * sizeof(unsigned char*));
	short def_color = COLOR_WHITE;
	transfer_ant(ant, old);
	for (i = 0; i < size; ++i) {
		c[i] = malloc(size * sizeof(unsigned char));
		for (j = 0; j < size; ++j) {
			c[i][j] = is_in_old_matrix(i, j, old) ? grid->c[i-old][j-old] : def_color;
		}
	}
	for (i = 0; i < grid->size; ++i){
		free(grid->c[i]);
	}
	free(grid->c);
	grid->c = c;
	grid->size = size;
}
