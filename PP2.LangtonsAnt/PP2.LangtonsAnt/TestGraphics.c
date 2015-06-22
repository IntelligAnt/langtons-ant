#include <stdio.h>
#include <stdlib.h>
#include <vld.h>
#include "logic.h"
#include "graphics.h"

#define DRAW_EVERY 1
#define INPUT_DELAY 100

int main(void)
{
	int init_size;
	printf("Pocetna velicina (2, 3, 4, 5, 6): ");
	scanf("%d", &init_size);
	assert(init_size == 2 || init_size == 3 || init_size == 4 || init_size == 5 || init_size == 6);
	system("cls");

	Colors *colors = colors_new(COLOR_SILVER);
	Grid *grid = grid_new(init_size, colors);
	Ant *ant = ant_new(grid, UP);
	int i = 1;
	short c, turn;
	//grid_make_sparse(grid);

	while (i) {
		printf("1. Nova boja.\n2. Izbrisi boju.\n0 za crtanje\n");
		scanf("%d", &i);
		switch (i) {
		case 0:
			if (!has_enough_colors(colors)) {
				i = 1;
			}
			break;
		case 1:
			printf("Unesite boju (0-14) i smer skretanja (-1 ili 1)\n");
			scanf("%hi %hi", &c, &turn);
			assert(abs(turn) == 1);
			if (!color_exists(colors, c)) {
				add_color(colors, c, turn);
			}
			break;
		case 2:
			printf("Unesite boju (0-14)\n");
			scanf("%hi", &c);
			if (color_exists(colors, c)) {
				remove_color(colors, c);
			}
			break;
		}
		system("cls");
	}

	init_graphics(COLOR_BLACK, COLOR_WHITE); // TODO fix flicker with bg colors other than white
	run_simulation(ant, grid, colors);

exit:
	colors_delete(colors);
	grid_delete(grid);
	ant_delete(ant);
	end_graphics();
	freopen("memleaks.dmp", "w", stdout);
	return 0;
}
