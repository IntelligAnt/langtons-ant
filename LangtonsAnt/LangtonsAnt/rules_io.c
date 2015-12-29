#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

Simulation *load_simulation(char *filename)
{
	Simulation *simulation = malloc(sizeof(Simulation));
	simulation->is_running = FALSE;
	FILE *input;
	int nn=0, i, j;
	char c;
	bool is_sparse;

	if (!(simulation->colors = load_rules(filename))) {
		return NULL;
	}

	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	while (nn < 5) {
		c = getc(input);
		if (c == '\n') {
			nn++;
		}
	}

	simulation->ant = malloc(sizeof(Ant));

	if (fscanf(input, "%d %d %d\n", &simulation->ant->pos.x, &simulation->ant->pos.y, &simulation->ant->dir) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%u\n", &simulation->steps) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%c\n", &is_sparse) < 0) {
		goto error_end;
	}

	simulation->grid = malloc(sizeof(Grid));
	simulation->grid->c = NULL;
	simulation->grid->tmp = NULL;
	simulation->grid->tmp_size = 0;
	simulation->grid->csr = NULL;


	if (fscanf(input, "%u %u %u %u\n", &simulation->grid->def_color, &simulation->grid->init_size, &simulation->grid->size, &simulation->grid->colored) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%d %d %d %d\n", &simulation->grid->top_left.x, &simulation->grid->top_left.y, &simulation->grid->bottom_right.x, &simulation->grid->bottom_right.y) < 0) {
		goto error_end;
	}

	if (is_sparse) {
		simulation->grid->csr = malloc(simulation->grid->size*sizeof(Cell*));
		size_t colp;
		Cell *temp;
		for (int i = 0; i < simulation->grid->size; i++) {
			if (feof(input)) {
				goto error_end;
			}

			temp = NULL;
			simulation->grid->csr[i]=NULL;
			while (fscanf(input, "%u%c", &colp, &c) > 0) {
				if (!temp) {
					simulation->grid->csr[i] = malloc(sizeof(Cell));
					simulation->grid->csr[i]->col_packed = colp;
					simulation->grid->csr[i]->next = NULL;
					temp = simulation->grid->csr[i];
				}
				else {
					temp->next= malloc(sizeof(Cell));
					temp->next->col_packed = colp;
					temp->next->next = NULL;
					temp = temp->next;
				}
			}
		}
	}
	else {
		simulation->grid->c = malloc(simulation->grid->size * sizeof(unsigned char*));
		for (i = 0; i < simulation->grid->size; i++) {
			simulation->grid->c[i] = malloc(simulation->grid->size);
			if (feof(input)) {
				goto error_end;
			}
			for (j = 0; j < simulation->grid->size; j++) {
				if (fscanf(input, (j == simulation->grid->size - 1) ? "%u\n" : "%u ", &simulation->grid->c[i][j]) < 0) {
					goto error_end;
				}
			}
		}
	}



	if (fclose(input) != NULL) {
		return NULL;
	}
	return simulation;

error_end: 
	simulation_delete(simulation);
	fclose(input);
	return NULL;

}

int save_simulation(char *filename, Simulation* simulation)
{
	int i, j;
	if (save_rules(filename, simulation->colors) == EOF) {
		return EOF;
	}

	FILE *output;
	short c;

	if (!(output = fopen(filename, "a"))) {
		return EOF;
	}

	if (fprintf(output, "%d %d %d\n", simulation->ant->pos.x, simulation->ant->pos.y, (int)simulation->ant->dir) < 0) {
		return EOF;
	}
	if (fprintf(output, "%u\n", simulation->steps) < 0) {
		return EOF;
	}
	if (fprintf(output, "%c\n", is_grid_sparse(simulation->grid)) < 0) {
		return EOF;
	}
	if (fprintf(output, "%u %u %u %u\n", simulation->grid->def_color, simulation->grid->init_size, simulation->grid->size, simulation->grid->colored) < 0) {
		return EOF;
	}
	if (fprintf(output, "%d %d %d %d\n", simulation->grid->top_left.x, simulation->grid->top_left.y, simulation->grid->bottom_right.x, simulation->grid->bottom_right.y) < 0) {
		return EOF;
	}
	Cell *temp;
	if (is_grid_sparse(simulation->grid)) {
		for (i = 0; i < simulation->grid->size; i++) {
			temp = simulation->grid->csr[i];
			while (temp) {
				if (fprintf(output, "%u ", temp->col_packed) < 0) {
					return EOF;
				}
				temp = temp->next;
			}
			if (fprintf(output, "\n") < 0) {
				return EOF;
			}
		}
	}
	else {
		for (i = 0; i < simulation->grid->size; i++) {
			for (j = 0; j < simulation->grid->size; j++) {
				if (fprintf(output, (j== simulation->grid->size-1) ? "%u\n" : "%u ", simulation->grid->c[i][j]) < 0) {
					return EOF;
				}
			}
		}
	}

	if (fclose(output) == EOF) {
		return EOF;
	}

}


Colors *load_rules(char *filename) // TODO format checks
{
	Colors *colors;
	FILE *input;
	int e;
	short def, c;
	
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
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->turn+c);
	}
	e += fscanf(input, "%hd %hd\n", &colors->first, &colors->last);
	e += fscanf(input, "%hd\n", &colors->n);

	fclose(input);

	if (e < RULES_TOTAL_FIELDS) {
		colors_delete(colors);
		return NULL;
	}

	return colors;
}

int save_rules(char *filename, Colors* colors)
{
	FILE *output;
	int e;
	short c;

	if (!(output = fopen(filename, "w"))) {
		return EOF;
	}

	e = fprintf(output, "%hd\n", colors->def);
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next[c]);
	}
	for (c = 0; c < COLOR_COUNT; ++c) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->turn[c]);
	}
	e += fprintf(output, "%hd %hd\n", colors->first, colors->last);
	e += fprintf(output, "%hd\n", colors->n);

	if (fclose(output) == EOF || e < RULES_TOTAL_FIELDS) {
		e = EOF;
	}
	return e;
}
