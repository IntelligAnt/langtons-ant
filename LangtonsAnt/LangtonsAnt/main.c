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
		fprintf(stderr, "usage: %s [rulesfile]\n", *argv);
		return;
	}
	if (argc == 2) {
		stgs.colors = load_rules(argv[1]);
	}
	if (!stgs.colors) {
		stgs.colors = colors_new(COLOR_SILVER);
	}

	stgs.init_size = GRID_DEF_INIT_SIZE;
	stgs.speed = LOOP_DEF_SPEED;
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);

	init_graphics(COLOR_BLACK, COLOR_WHITE);
	game_loop();
	end_graphics();

	simulation_delete(stgs.linked_sim);
	colors_delete(stgs.colors);
	
	return EXIT_SUCCESS;
}
