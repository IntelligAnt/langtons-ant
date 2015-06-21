#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logic.h"
#include "graphics.h"

// SuperPak
void grid_silent_expand_it_because_i_can(Grid *grid)
{
	while (grid->temp_buffer_allocated_rows < grid->size * 3) {
		grid->temp_buffer[grid->temp_buffer_allocated_rows++] = malloc(grid->size * 3);
	}
}

Grid *grid_new(size_t size, Colors *colors)
{
	Grid *grid = malloc(sizeof(Grid));
	size_t i, j;
	grid->c = malloc(size * sizeof(unsigned char*));
	for (i = 0; i < size; ++i) {
		grid->c[i] = malloc(size * sizeof(unsigned char));
		memset(grid->c[i], (unsigned char)colors->def, size);
	}
	grid->size = grid->init_size = size;
	grid->rows = NULL;
	grid->temp_buffer = NULL;
	grid->temp_buffer_allocated_rows = 0;
	grid->def_color = (unsigned char)colors->def;
	return grid;
}

static void grid_delete_n(Grid *grid)
{
	int i;
	for (i = 0; i < grid->size; ++i) {
		free(grid->c[i]);
	}
	free(grid->c);
}

static void grid_delete_s(Grid *grid)
{
	int i;
	Cell *temp;
	for (i = 0; i < grid->size; i++) {
		while (grid->rows[i]) {
			temp = grid->rows[i];
			grid->rows[i] = (grid->rows[i])->next;
			free(temp);
		}
	}
	free(grid->rows);
}

void grid_delete(Grid *grid)
{
	if (is_grid_sparse(grid)) {
		grid_delete_s(grid);
	} else {
		grid_delete_n(grid);
	}
	free(grid);
}

static void transfer_ant(Ant *ant, size_t old_size)
{
	ant->pos.y += old_size;
	ant->pos.x += old_size;
}


static bool is_in_old_matrix(int y, int x, size_t old_size)
{
	return y >= old_size && y < 2*old_size && x >= old_size && x < 2*old_size;
}

static bool is_in_old_matrix_row(int y, size_t old_size)
{
	return y >= old_size && y < 2 * old_size;
}

static void grid_expand_n(Grid *grid)
{
	size_t old = grid->size, size = old*GRID_MUL, i;
	size_t pre = (GRID_MUL/2)*old, post = (GRID_MUL/2+1)*old;
	/*unsigned char **c = malloc(size * sizeof(unsigned char*));

	for (i = 0; i < size; ++i) {
		c[i] = malloc(size * sizeof(unsigned char));
		for (j = 0; j < size; ++j) {
			c[i][j] = is_in_old_matrix(i, j, old) ? grid->c[i-old][j-old] : grid->def_color;
		}
	}
	grid_delete_n(grid);
	grid->c = c;
	grid->size = size;*/
	grid_silent_expand_it_because_i_can(grid);
	for (i = 0; i < size; ++i) {
		memset(grid->temp_buffer[i], grid->def_color, size);
		if (i >= pre && i < post) {
			memcpy(&grid->temp_buffer[old+i][old], grid->c[i-old], old);
		}
		/*for (j = 0; j < size; ++j) {
			grid->temp_buffer[i][j] = is_in_old_matrix(i, j, old) ? grid->c[i-old][j-old] : grid->def_color;
		}*/
	}
	grid_delete_n(grid);
	grid->c = grid->temp_buffer;
	grid->temp_buffer = NULL;
	grid->temp_buffer_allocated_rows = 0;
	grid->size = size;
}

static void grid_expand_s(Grid *grid)
{
	size_t old = grid->size, size = GRID_MUL*old, i;
	Cell **new = malloc(size*sizeof(Cell*)), *t;

	for (i = 0; i < size; ++i) {
		if (is_in_old_matrix_row(i, old)) {
			new[i] = grid->rows[i-old];
			grid->rows[i-old] = NULL;
			t=new[i];
			while (t) {
				t->column += old;
				t = t->next;
			}
		} else {
			new[i] = NULL;
		}
	}
	free(grid->rows);
	grid->rows = new;
	grid->size = size;
}

void grid_expand(Grid *grid, Ant *ant)
{
	transfer_ant(ant, grid->size);
	if (!is_grid_sparse(grid) && grid->size*GRID_MUL > GRID_MAX_N_SIZE) {
		//grid_make_sparse(grid);
	}
	if (is_grid_sparse(grid)) {
		grid_expand_s(grid);
	} else {
		grid_expand_n(grid);
	}
}

void grid_make_sparse(Grid *grid)
{
	size_t size = grid->size;
	int i, j;
	Cell **cur;
	char c;
	grid->rows = malloc(size*sizeof(Cell*));
	for (i = 0; i < size; i++) {
		grid->rows[i] = NULL;
		cur = grid->rows + i;
		for (j = 0; j < size; j++) {
			c = grid->c[i][j];
			if (c != grid->def_color) {
				new_cell(cur, j, c);
				cur = &((*cur)->next);
			}
		}
	}
	grid_delete_n(grid);
	grid->c = NULL;
}

bool is_grid_sparse(Grid *grid)
{
	return grid->rows ? assert(!grid->c), 1 : 0;
}

void new_cell(Cell **cur, unsigned column, unsigned char c)
{
	Cell *new = malloc(sizeof(Cell));
	new->column = column;
	new->c = c;
	new->next = *cur;
	*cur = new;
}

unsigned char color_at_s(Grid *grid, Vector2i p)
{
	Cell *t = grid->rows[p.y];
	while (t && t->column < p.x) {
		t = t->next;
	}
	return (!t || t->column != p.x) ? grid->def_color : t->c;
}

// SuperPak
void grid_silent_expand(Grid *grid)
{
#define SILENT_EXPAND_STEP 5
	int i;
	if (!grid->temp_buffer) {
		grid->temp_buffer = malloc(grid->size * GRID_MUL * sizeof(unsigned char*));
		grid->temp_buffer_allocated_rows = 0;
	}
	if (is_grid_sparse(grid)) {
		return;
	}
	for (i = 0; i < SILENT_EXPAND_STEP; i++) {
		if (grid->temp_buffer_allocated_rows == grid->size*GRID_MUL) {
			return;
		}
		grid->temp_buffer[grid->temp_buffer_allocated_rows] = malloc(grid->size*GRID_MUL);
		++grid->temp_buffer_allocated_rows;
	}
}
