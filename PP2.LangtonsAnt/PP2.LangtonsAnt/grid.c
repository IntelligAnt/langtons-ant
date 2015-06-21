#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logic.h"
#include "graphics.h"

Grid *grid_new(size_t size, Colors *colors)
{
	Grid *grid = malloc(sizeof(Grid));
	size_t i, j;
	grid->c = malloc(size * sizeof(unsigned char*));
	for (i = 0; i < size; ++i) {
		grid->c[i] = malloc(size * sizeof(unsigned char));
		memset(grid->c[i], (unsigned char)colors->def, size);
	}
	grid->csr = NULL;
	grid->size = grid->init_size = size;
	grid->tmp = NULL;
	grid->tmp_size = 0;
	grid->def_color = (unsigned char)colors->def;
	grid->top_left.y = grid->top_left.x = grid->size / 2;
	grid->bottom_right = grid->top_left;
	grid->used = 0;
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
		while (grid->csr[i]) {
			temp = grid->csr[i];
			grid->csr[i] = (grid->csr[i])->next;
			free(temp);
		}
	}
	free(grid->csr);
}

static void grid_delete_tmp(Grid *grid)
{
	free(grid->tmp);
	grid->tmp_size = 0;
}

void grid_delete(Grid *grid)
{
	if (is_grid_sparse(grid)) {
		grid_delete_s(grid);
	} else {
		grid_delete_n(grid);
		grid_delete_tmp(grid);
	}
	free(grid);
}

static void transfer_vector(Vector2i *v, size_t old_size)
{
	v->y += old_size;
	v->x += old_size;
}

static bool is_in_old_matrix(int y, int x, size_t old_size)
{
	return y >= old_size && y < 2*old_size && x >= old_size && x < 2*old_size;
}

static bool is_in_old_matrix_row(int y, size_t old_size)
{
	return y >= old_size && y < 2 * old_size;
}

void grid_silent_expand(Grid *grid)
{
	int size = grid->size*GRID_MUL, i;
	if (is_grid_sparse(grid) || grid->tmp_size >= size) {
		return;
	}
	if (!grid->tmp) {
		grid->tmp = malloc(size * sizeof(unsigned char*));
		grid->tmp_size = 0;
	}
	for (i = 0; i < GRID_MAX_SILENT_EXPAND && grid->tmp_size < size; i++) {
		grid->tmp[grid->tmp_size++] = malloc(size);
	}
}

static void grid_expand_n(Grid *grid)
{
	size_t old = grid->size, size = old*GRID_MUL, i;
	size_t pre = old*(GRID_MUL/2), post = old*(GRID_MUL/2+1);
	
	//grid_fill_tmp(grid);
	for (i = 0; i < size; ++i) {
		memset(grid->tmp[i], grid->def_color, size);
		if (i >= pre && i < post) {
			memcpy(&grid->tmp[i][pre], grid->c[i-pre], old);
		}
	}
	
	grid_delete_n(grid);
	grid->c = grid->tmp;
	grid->tmp = NULL;
	grid->tmp_size = 0;
	grid->size = size;
}

static void grid_expand_s(Grid *grid)
{
	size_t old = grid->size, size = GRID_MUL*old, i;
	Cell **new = malloc(size*sizeof(Cell*)), *t;

	for (i = 0; i < size; ++i) {
		if (is_in_old_matrix_row(i, old)) {
			new[i] = grid->csr[i-old];
			grid->csr[i-old] = NULL;
			t = new[i];
			while (t) {
				t->column += old;
				t = t->next;
			}
		} else {
			new[i] = NULL;
		}
	}
	free(grid->csr);
	grid->csr = new;
	grid->size = size;
}

void grid_expand(Grid *grid, Ant *ant)
{
	transfer_vector(&ant->pos, grid->size);
	if (!is_grid_sparse(grid)) {
		transfer_vector(&grid->top_left, grid->size);
		transfer_vector(&grid->bottom_right, grid->size);
		if (grid->size*GRID_MUL > GRID_SIZE_THRESHOLD) {
			grid_make_sparse(grid);
		}
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
	grid->csr = malloc(size * sizeof(Cell*));
	for (i = 0; i < size; i++) {
		grid->csr[i] = NULL;
		cur = grid->csr + i;
		for (j = 0; j < size; j++) {
			c = grid->c[i][j];
			if (c != grid->def_color) {
				new_cell(cur, j, c);
				cur = &((*cur)->next);
			}
		}
	}
	grid_delete_n(grid);
	grid_delete_tmp(grid);
	grid->c = NULL;
}

bool is_grid_sparse(Grid *grid)
{
	return grid->csr ? assert(!grid->c), 1 : 0;
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
	Cell *t = grid->csr[p.y];
	while (t && t->column < p.x) {
		t = t->next;
	}
	return (!t || t->column != p.x) ? grid->def_color : t->c;
}
