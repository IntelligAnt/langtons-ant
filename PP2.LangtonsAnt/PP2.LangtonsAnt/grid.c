#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"
#include "graphics.h"

Grid *grid_new(size_t size, Colors *colors)
{
	Grid *grid = malloc(sizeof(Grid));
	unsigned char **c = malloc(size * sizeof(unsigned char*));
	size_t i, j;
	for (i = 0; i < size; ++i) {
		c[i] = malloc(size * sizeof(unsigned char));
		for (j = 0; j < size; ++j) {
			c[i][j] = (unsigned char)colors->def;;
		}
	}
	grid->c = c;
	grid->size = size;
	return grid;
}

void grid_delete(Grid *grid)
{
	size_t i;
	for (i = 0; i < grid->size; ++i) {
		free(grid->c[i]);
	}
	free(grid->c);
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

void expand_grid(Grid *grid, Ant *ant, Colors *colors)
{
	size_t old = grid->size, size = GRID_MUL*old, i, j;
	unsigned char **c = malloc(size * sizeof(unsigned char*));
	transfer_ant(ant, old);
	for (i = 0; i < size; ++i) {
		c[i] = malloc(size * sizeof(unsigned char));
		for (j = 0; j < size; ++j) {
			c[i][j] = is_in_old_matrix(i, j, old) ? grid->c[i-old][j-old]
				                                  : (unsigned char) colors->def;
		}
	}
	for (i = 0; i < grid->size; ++i){
		free(grid->c[i]);
	}
	free(grid->c);
	grid->c = c;
	grid->size = size;
}

//SPARSE

void new_element(Element *cur, unsigned column, unsigned char c)
{
	Element *new = malloc(sizeof(Element));
	new->column = column;
	new->c = c;
	new->prev = cur;
	if (!cur) {
		new->next = cur;
		cur = new;
	} else {
		new->next = cur->next;
		cur->next = new;
		if (new->next) {
			new->next->prev = new;
		}
	}
}

Grid_s *to_sparse(Grid *old, Colors *colors)
{
	int size = old->size;
	Grid_s *new = malloc(sizeof(Grid_s));
	new->size = size;
	new->rows = malloc(size*sizeof(Element*));

	int i,j;
	Element *cur;
	char c;
	for (i = 0; i < size; i++){
		cur = new->rows[i];
		for (j = 0; j < size; j++){
			c = old->c[i][j];
			if (c == colors->def) {
				new_element(cur, j, c);
			}
		}
	}
	grid_delete(old);
	return new;
}

void list_delete(Element **list, size_t size)
{
	int i;
	Element *temp;
	for (i = 0; i < size; i++) {
		while (list[i]) {
			temp = list[i];
			list[i] = (list[i])->next;
			free(temp);
		}
	}
	free(list);
}

void grid_s_delete(Grid_s *grid)
{
	list_delete(grid->rows, grid->size);
	free(grid);
}

static bool is_in_old_matrix_row(int y, size_t old_size)
{
	return y >= old_size && y < 2 * old_size;
}

void expand_grid_s(Grid_s *grid, Ant *ant)
{
	size_t old = grid->size, size = GRID_MUL*old, i;
	grid->size= size;
	Element **new = malloc(size*sizeof(Element*));
	
	transfer_ant(ant, old);
	for (i = 0; i < size; ++i) {
		new[i] = is_in_old_matrix_row(i, old) ? grid->rows + i - old : NULL;
	}
	list_delete(grid->rows, old);
	grid->rows = new;
}