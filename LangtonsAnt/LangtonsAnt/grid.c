#include <stdlib.h>
#include <string.h>

#include "logic.h"
#include "graphics.h"

Grid *grid_new(Colors *colors, size_t init_size)
{
	Grid *grid = malloc(sizeof(Grid));
	size_t i;
	grid->c = malloc(init_size * sizeof(byte*));
	for (i = 0; i < init_size; i++) {
		grid->c[i] = malloc(init_size);
		memset(grid->c[i], (byte)colors->def, init_size);
	}
	grid->csr = NULL;
	grid->size = grid->init_size = init_size;
	grid->tmp = NULL;
	grid->tmp_size = 0;
	grid->def_color = (byte)colors->def;
	grid->top_left.y = grid->top_left.x = init_size / 2;
	grid->bottom_right = grid->top_left;
	grid->colored = 0;
	return grid;
}

static void grid_delete_tmp(Grid *grid)
{
	size_t i;
	if (grid->tmp) {
		for (i = 0; i < grid->tmp_size; i++) {
			if (grid->tmp[i]) {
				free(grid->tmp[i]);
			}
		}
		free(grid->tmp);
	}
	grid->tmp_size = 0;
}

static void grid_delete_n(Grid *grid)
{
	size_t i;
	grid_delete_tmp(grid);
	for (i = 0; i < grid->size; i++) {
		free(grid->c[i]);
	}
	free(grid->c);
}

static void grid_delete_s(Grid *grid)
{
	size_t i;
	Cell *cell;
	for (i = 0; i < grid->size; i++) {
		while (grid->csr[i]) {
			cell = grid->csr[i];
			grid->csr[i] = (grid->csr[i])->next;
			free(cell);
		}
	}
	free(grid->csr);
}

void grid_delete(Grid *grid)
{
	is_grid_sparse(grid) ? grid_delete_s(grid) : grid_delete_n(grid);
	free(grid);
}

static void transfer_vector(Vector2i *v, size_t old_size)
{
	v->y += old_size;
	v->x += old_size;
}

static bool is_in_old_matrix_row(size_t y, size_t old_size)
{
	return y >= old_size && y < 2*old_size;
}

void grid_silent_expand(Grid *grid)
{
	size_t size = grid->size*GRID_MULT, i;
	if (is_grid_sparse(grid) || grid->tmp_size >= size) {
		return;
	}
	if (!grid->tmp) {
		grid->tmp = malloc(size * sizeof(byte*));
		grid->tmp_size = 0;
	}
	for (i = 0; i < GRID_MAX_SILENT_EXPAND && grid->tmp_size < size; i++) {
		grid->tmp[grid->tmp_size++] = malloc(size);
	}
}

static void grid_fill_tmp(Grid *grid)
{
	size_t size = grid->size * GRID_MULT;
	if (!grid->tmp) {
		grid->tmp = malloc(size * sizeof(byte*));
		grid->tmp_size = 0;
	}
	while (grid->tmp_size < size) {
		grid->tmp[grid->tmp_size++] = malloc(size);
	}
}

static void grid_expand_n(Grid *grid)
{
	size_t old = grid->size, size = old*GRID_MULT, i;
	size_t pre = old*(GRID_MULT/2), post = old*(GRID_MULT/2+1);
	
	grid_fill_tmp(grid);
	for (i = 0; i < size; i++) {
		memset(grid->tmp[i], grid->def_color, size);
		if (i >= pre && i < post) {
			memcpy(&grid->tmp[i][pre], grid->c[i-pre], old);
			free(grid->c[i-pre]);
		}
	}
	free(grid->c);
	
	grid->c = grid->tmp;
	grid->tmp = NULL;
	grid->tmp_size = 0;
	grid->size = size;
}

static void grid_expand_s(Grid *grid)
{
	size_t old = grid->size, size = GRID_MULT*old, i;
	Cell **new = malloc(size*sizeof(Cell*)), *t;

	for (i = 0; i < size; i++) {
		if (is_in_old_matrix_row(i, old)) {
			new[i] = grid->csr[i-old];
			grid->csr[i-old] = NULL;
			t = new[i];
			while (t) {
				CELL_SET_COLUMN(t, CELL_GET_COLUMN(t) + old);
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
	transfer_vector(&grid->top_left, grid->size);
	transfer_vector(&grid->bottom_right, grid->size);
	if (!is_grid_sparse(grid)) {
		if (grid->size*GRID_MULT > GRID_SIZE_THRESHOLD && GRID_EFFICIENCY(grid) < 1) {
			grid_make_sparse(grid);
			grid_expand_s(grid);
		} else {
			grid_expand_n(grid);
		}
	} else {
		grid_expand_s(grid);
	}
}

void grid_make_sparse(Grid *grid)
{
	size_t size = grid->size, i, j;
	Cell **cur;
	byte c;

	grid_delete_tmp(grid);

	grid->csr = calloc(size, sizeof(Cell*));
	for (i = 0; i < size; i++) {
		cur = grid->csr + i;
		for (j = 0; j < size; j++) {
			c = grid->c[i][j];
			if (c != grid->def_color) {
				new_cell(cur, j, c);
				cur = &(*cur)->next;
			}
		}
		free(grid->c[i]);
	}
	free(grid->c);
	grid->c = NULL;
}

bool is_grid_sparse(Grid *grid)
{
	return grid->csr ? assert(!grid->c), TRUE : FALSE;
}

bool is_grid_usage_low(Grid *grid)
{
	int b = (grid->bottom_right.y - grid->top_left.y + 1) * (grid->bottom_right.x - grid->top_left.x + 1);
	return (double)grid->colored / b < GRID_USAGE_THRESHOLD;
}

void new_cell(Cell **cur, size_t column, byte c)
{
	Cell *new = malloc(sizeof(Cell));
	CELL_SET_COLUMN(new, column);
	CELL_SET_COLOR(new, c);
	new->next = *cur;
	*cur = new;
}

byte color_at_s(Grid *grid, Vector2i p)
{
	size_t x = p.x;
	Cell *t = grid->csr[p.y];
	while (t && CELL_GET_COLUMN(t) < x) {
		t = t->next;
	}
	return (!t || CELL_GET_COLUMN(t) != x) ? grid->def_color : CELL_GET_COLOR(t);
}
