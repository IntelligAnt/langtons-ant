#include <assert.h>
#include <stdlib.h>
#include "logic.h"
#include "graphics.h"

#define DRAW_EVERY 1

int main(void)
{
	int init_size;
	printf("Pocetna velicina (2, 3 ili 5): ");
	scanf("%d", &init_size);
	assert(init_size == 2 || init_size == 3 || init_size == 5);
	system("cls");

	Colors *colors = init_colors(COLOR_WHITE);
	Grid *grid = grid_new(init_size, colors);
	Ant *ant = ant_new(grid, UP);
	int i = 1;
	short c, turn;
	
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
			scanf("%hi %hi", &c, &turn);
			assert(abs(turn) == 1);
			if (!color_exists(colors, c)) {
				new_color(colors, c, turn);
			}
			break;
		case 2:
			printf("Unesite boju (0-14)\n");
			scanf("%hi", &c);
			if (color_exists(colors, c)) {
				delete_color(colors, c);
			}
			break;
		}
		system("cls");
	}

	Vector2i oldp;
	bool in_bounds;
	int steps = 0, cnt = DRAW_EVERY-1, input_delay = 0, ch;

	init_graphics(COLOR_BLACK, COLOR_WHITE); // TODO fix flicker with bg colors other than white
	draw_grid_full(grid);

	while (1) {
		/*if (steps == 200) {
			grid_to_sparse(grid);
		}*/
		oldp = ant->pos;
		in_bounds = ant_move(ant, grid, colors);
		if (!in_bounds) {
			grid_expand(grid, ant);
			draw_grid_full(grid);
		} else if (++cnt == DRAW_EVERY) {
			draw_grid_iter(grid, oldp, ant->pos);
			cnt = 0;
		}
		if (input_delay == 0) {
			switch (ch = getch()) {
			case KEY_UP:
				scroll_grid(grid, -1, 0);
				goto delay;
			case KEY_DOWN:
				scroll_grid(grid, 1, 0);
				goto delay;
			case KEY_LEFT:
				scroll_grid(grid, 0, -1);
				goto delay;
			case KEY_RIGHT:
				scroll_grid(grid, 0, 1);
			delay:
				input_delay = 2;
				draw_grid_full(grid);
			}
		} else {
			--input_delay;
		}
		++steps;
	}

	end_graphics();

	return 0;
}
