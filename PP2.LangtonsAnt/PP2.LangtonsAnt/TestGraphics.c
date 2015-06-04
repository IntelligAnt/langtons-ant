#include "graphics.h"

int main(void)
{
	Colors *colors = init_colors(COLOR_WHITE);
	Grid *grid = grid_new(GRID_SIZE_SMALL);
	Ant *ant = ant_new(grid, UP);
	init_graphics(colors->def);
	end_graphics();
}