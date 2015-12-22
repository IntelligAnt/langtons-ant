#include <stdio.h>
#include <stdlib.h>

#include "logic.h"
#include "graphics.h"
#include "io.h"

int main(int argc, char *argv[])
{
	Colors *colors = NULL;

	if (argc > 2) {
		printf("Usage: pp2.langtonsant.exe [rulesfile]\n");
		return;
	}
	if (argc == 2) {
		colors = load_rules(argv[1]);
	}

	if (!colors) {
		stgs.colors = colors_new(COLOR_SILVER);
	}
	stgs.init_size = GRID_DEF_INIT_SIZE;
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);

	init_graphics(COLOR_BLACK, COLOR_WHITE);
	draw_loop();

	simulation_delete(stgs.linked_sim);
	colors_delete(stgs.colors);
	end_graphics();
	
	return EXIT_SUCCESS;
}
