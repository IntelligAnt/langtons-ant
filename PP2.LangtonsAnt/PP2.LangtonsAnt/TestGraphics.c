#include "logic.h"

int main(void)
{
	Colors *colors = init_colors(COLOR_WHITE);
	Grid *grid = grid_new(INIT_SIZE);
	Ant *ant = ant_new(grid, UP);
	//init_graphics(colors->def);
}