#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "logic.h"

#define DRAW_EVERY 1
//#define DELAY 0

#define CLEAR() system("cls");

void draw_grid(Grid *grid, int steps)
{
	int i, j;

	CLEAR();

	for (i = 0; i < grid->size; ++i) {
		for (j = 0; j < grid->size; ++j) {
			putchar(grid->c[i][j]);
		}
		putchar('\n');
	}

	printf("Steps: %d\n", steps);
}

int main(void)
{
	Colors *colors = init_colors(COLOR_WHITE);
	Grid *grid = grid_new(INIT_SIZE);
	Ant *ant = ant_new(grid, UP);
	int i = 1, steps, cnt = DRAW_EVERY-1;
	bool in_bounds;

	short color, turn;
	while (i) {
		printf("1. Nova boja.\n2. Izbrisi boju.\n0 za crtanje\n");
		scanf("%d", &i);
		switch (i) {
		case 0: 
			if (!enough_colors(colors)) {
				i = 1;
			}
			break;
		case 1:
			printf("Unesite boju (0-14) i smer skretanja (-1 ili 1)\n");
			scanf("%hi %hi", &color, &turn);
			assert(abs(turn) == 1);
			if (!color_exists(colors, color)) {
				new_color(colors, color, turn);
			}
			break;
		case 2:
			printf("Unesite boju (0-14)\n");
			scanf("%hi", &color);
			if (color_exists(colors, color)) {
				delete_color(colors, color);
			}
			break;
		}

		CLEAR();
	}

	for (steps = 0; steps < 1000; ++steps) {
		if (++cnt == DRAW_EVERY) {
			draw_grid(grid, steps);
			cnt = 0;
		}
		in_bounds = ant_move(ant, grid, colors);
		if (!in_bounds) {
			expand_grid(grid, ant);
			// draw_grid_full()
		}
		//Sleep(DELAY);
	}

	return 0;
}
