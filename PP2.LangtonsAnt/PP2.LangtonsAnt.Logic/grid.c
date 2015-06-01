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
	ant->p.x += old_size;
	ant->p.y += old_size;
}

static bool is_in_old_matrix(int x, int y, unsigned old_size)
{
	return x >= old_size && x < 2*old_size && y >= old_size && y < 2*old_size;
}

void expand_grid(Grid *grid, Ant *ant)
{
	unsigned old = grid->size, size = 3*old, i, j;
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