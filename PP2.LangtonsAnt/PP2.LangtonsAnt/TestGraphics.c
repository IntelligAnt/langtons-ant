#include <stdio.h>
#include <stdlib.h>
//#include <vld.h>
#include "logic.h"
#include "graphics.h"

#define DRAW_EVERY 1
#define INPUT_DELAY 100

int main(void)
{
	sim.colors = colors_new(COLOR_SILVER);
	init_graphics(COLOR_BLACK, COLOR_WHITE); // TODO fix flicker with bg colors other than white
	//run_simulation(ant, grid, colors);

//exit:
//	colors_delete(colors);
//	grid_delete(grid);
//	ant_delete(ant);
//	end_graphics();
	freopen("memleaks.dmp", "w", stdout);
	return 0;
}
