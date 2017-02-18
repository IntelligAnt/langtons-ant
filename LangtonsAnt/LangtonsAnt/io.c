#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

Colors *load_rules(char *filename) // TODO format checks
{
	Colors *colors;
	FILE *input;
	int e;
	color_t def, c;

	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	e = fscanf(input, "%hd\n", &def);
	if (def < 0 || def >= COLOR_COUNT) {
		return NULL;
	}
	colors = colors_new(def);
	colors->def = def;

	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next+c);
	}
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%c\n" : "%c ", colors->turn+c);
	}
	e += fscanf(input, "%hd %hd\n", &colors->first, &colors->last);
	e += fscanf(input, "%zu\n", &colors->n);

	if (fclose(input) == EOF) {
		return NULL;
	}

	if (e < RULES_TOTAL_FIELDS) {
		colors_delete(colors);
		return NULL;
	}

	return colors;
}

int save_rules(char *filename, Colors *colors)
{
	FILE *output;
	int e;
	color_t c;

	if (!(output = fopen(filename, "w"))) {
		return EOF;
	}

	e = fprintf(output, "%hd\n", colors->def);
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next[c]);
	}
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%c\n" : "%c ", colors->turn[c]);
	}
	e += fprintf(output, "%hd %hd\n", colors->first, colors->last);
	e += fprintf(output, "%zu\n", colors->n);

	if (fclose(output) == EOF || e < RULES_TOTAL_FIELDS) {
		e = EOF;
	}
	return e;
}

Simulation *load_simulation(char *filename)
{
	Simulation *sim;
	Colors *colors;
	FILE *input;
	size_t nn = 0, i, j;
	bool is_sparse;

	if (!(colors = load_rules(filename))) {
		return NULL;
	}

	sim = simulation_new(colors, GRID_DEF_INIT_SIZE);

	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	while (nn < 5) {
		int c = getc(input);
		if (c == '\n') {
			nn++;
		}
	}

	if (fscanf(input, "%d %d %u\n", &sim->ant->pos.x, &sim->ant->pos.y,
			   &sim->ant->dir) < 3) {
		return sim;
	}
	if (fscanf(input, "%zu\n", &sim->steps) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%c\n", &is_sparse) < 0) {
		goto error_end;
	}

	grid_delete(sim->grid);
	sim->grid = malloc(sizeof(Grid));
	sim->grid->c = NULL;
	sim->grid->tmp = NULL;
	sim->grid->tmp_size = 0;
	sim->grid->csr = NULL;

	if (fscanf(input, "%hhu %zu %zu %zu\n", &sim->grid->def_color,
			   &sim->grid->init_size, &sim->grid->size, &sim->grid->colored) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%d %d %d %d\n", &sim->grid->top_left.x, &sim->grid->top_left.y,
			   &sim->grid->bottom_right.x, &sim->grid->bottom_right.y) < 0) {
		goto error_end;
	}

	if (is_sparse) {
		sim->grid->csr = malloc(sim->grid->size*sizeof(Cell*));
		size_t colp;
		Cell *temp;
		for (i = 0; i < sim->grid->size; i++) {
			char c;
			temp = NULL;
			sim->grid->csr[i] = NULL;
			while (fscanf(input, "%c", &c) > 0 && c == ' ') {
				if (fscanf(input, "%zu", &colp) < 0) {
					goto error_end;
				}
				if (!temp) {
					sim->grid->csr[i] = malloc(sizeof(Cell));
					sim->grid->csr[i]->packed = colp;
					sim->grid->csr[i]->next = NULL;
					temp = sim->grid->csr[i];
				} else {
					temp->next = malloc(sizeof(Cell));
					temp->next->packed = colp;
					temp->next->next = NULL;
					temp = temp->next;
				}
			}
		}
	} else {
		sim->grid->c = malloc(sim->grid->size * sizeof(byte*));
		for (i = 0; i < sim->grid->size; i++) {
			sim->grid->c[i] = malloc(sim->grid->size);
			if (feof(input)) {
				goto error_end;
			}
			for (j = 0; j < sim->grid->size; j++) {
				if (fscanf(input, (j == sim->grid->size - 1) ? "%hhu\n" : "%hhu ",
						   &sim->grid->c[i][j]) < 0) {
					goto error_end;
				}
			}
		}
	}

	if (fclose(input) == EOF) {
		return NULL;
	}
	return sim;

error_end:
	simulation_delete(sim);
	fclose(input);
	return NULL;
}

int save_simulation(char *filename, Simulation *sim)
{
	size_t i, j;
	if (save_rules(filename, sim->colors) == EOF) {
		return EOF;
	}

	FILE *output;

	if (!(output = fopen(filename, "a"))) {
		return EOF;
	}

	if (fprintf(output, "%d %d %u\n", sim->ant->pos.x, sim->ant->pos.y,
				sim->ant->dir) < 0) {
		return EOF;
	}
	if (fprintf(output, "%zu\n", sim->steps) < 0) {
		return EOF;
	}
	if (fprintf(output, "%c\n", is_grid_sparse(sim->grid)) < 0) {
		return EOF;
	}
	if (fprintf(output, "%hhu %zu %zu %zu\n", sim->grid->def_color,
				sim->grid->init_size, sim->grid->size, sim->grid->colored) < 0) {
		return EOF;
	}
	if (fprintf(output, "%d %d %d %d\n", sim->grid->top_left.x, sim->grid->top_left.y,
				sim->grid->bottom_right.x, sim->grid->bottom_right.y) < 0) {
		return EOF;
	}
	Cell *temp;
	if (is_grid_sparse(sim->grid)) {
		for (i = 0; i < sim->grid->size; i++) {
			temp = sim->grid->csr[i];
			while (temp) {
				if (fprintf(output, " %zu", temp->packed) < 0) {
					return EOF;
				}
				temp = temp->next;
			}
			if (fprintf(output, "\n") < 0) {
				return EOF;
			}
		}
	} else {
		for (i = 0; i < sim->grid->size; i++) {
			for (j = 0; j < sim->grid->size; j++) {
				if (fprintf(output, (j == sim->grid->size-1) ? "%hhu\n" : "%hhu ",
							sim->grid->c[i][j]) < 0) {
					return EOF;
				}
			}
		}
	}

	if (fclose(output) == EOF) {
		return EOF;
	}

	return 0; // TODO return # of fields read
}
