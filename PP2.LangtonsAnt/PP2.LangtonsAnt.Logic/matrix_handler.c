#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"

void move_ant(Ant *ant, unsigned old_size)
{
	ant->p.x += old_size;
	ant->p.y += old_size;
}

void expand_grid(Grid *grid, Ant *ant)
{
	unsigned size = grid->size * 3;
	unsigned char **new = malloc(size * sizeof(unsigned char*));
	short def_color = COLOR_WHITE;
	unsigned i, j;
	move_ant(ant, grid->size);
	for (i = 0; i < size; ++i) {
		new[i] = malloc(size * sizeof(unsigned char));
		for (j = 0; j < size; ++j) {
			new[i][j] = i >= grid->size && i < 2 * grid->size && j >= grid->size && j < 2 * grid->size ? grid->c[i - grid->size][j - grid->size] : def_color;
		}
	}
	for (i = 0; i < grid->size; i++){
		free(grid->c[i]);
	}
	free(grid->c);
	grid->c = new;
	grid->size = size;
}

int ant_overboard(Ant *ant, Grid *grid)
{
	return ant->p.x < 0 || ant->p.x >= grid->size || ant->p.y < 0 || ant->p.y >= grid->size;
}