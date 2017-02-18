#include <stdio.h>
#include <stdlib.h>
#ifndef NDEBUG
#include <vld.h>
#endif

#include "graphics.h"
#include "io.h"

int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "usage: %s [simulation_file]\n", *argv);
		return EXIT_FAILURE;
	}

	stgs.init_size = GRID_DEF_INIT_SIZE;
	stgs.speed = LOOP_DEF_SPEED;

	if (argc == 2 && (stgs.linked_sim = load_simulation(argv[1]))) {
		stgs.colors = stgs.linked_sim->colors;
	} else {
		stgs.colors = colors_new(COLOR_SILVER);
		stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	}
	
	init_graphics(COLOR_BLACK, COLOR_WHITE);
	game_loop();
	end_graphics();

	simulation_delete(stgs.linked_sim);
	colors_delete(stgs.colors);
	
	return EXIT_SUCCESS;
}
