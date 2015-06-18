#include "graphics.h"

int main(void)
{
	Colors *colors = init_colors(COLOR_WHITE);
	Grid *grid = grid_new(243);
	Ant *ant = ant_new(grid, UP);
	
	init_graphics(COLOR_BLACK, COLOR_WHITE);
	draw_grid_full(grid);
	end_graphics();

	return 0;
}
