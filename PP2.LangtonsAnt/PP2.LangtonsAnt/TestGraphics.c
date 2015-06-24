#include <stdio.h>
#include <stdlib.h>
//#include <vld.h>
#include "logic.h"
#include "graphics.h"

#define DRAW_EVERY 1
#define INPUT_DELAY 100

int main(void)
{
	stgs.colors = colors_new(COLOR_SILVER);
	stgs.init_size = 4;
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);

	init_graphics(COLOR_BLACK, COLOR_WHITE);
	draw_loop();

exit:
	colors_delete(stgs.colors);
	end_graphics();
	
	return EXIT_SUCCESS;
}
