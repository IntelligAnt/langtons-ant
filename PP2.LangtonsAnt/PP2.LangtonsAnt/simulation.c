#include "logic.h"
#include "graphics.h"

#define INPUT_DELAY 100
#define FPS 30

Simulation sim;

static void handle_input(Ant *ant, Grid *grid)
{
	static int input_delay = 0;
	int ch;
	if (input_delay == 0) {
		ch = getch();
		if (grid_key_command(grid, ant, ch) != ERR) { // TODO wgetch refreshes
			draw_grid_full(grid);
		}
		menu_key_command(ch);
		input_delay += INPUT_DELAY;
	} else {
		--input_delay;
	}
}

void run_simulation()
{
	static size_t steps = 0;
	Vector2i oldp;
	clock_t last;
	float time_for_frame = 1.0f / FPS;

	assert(sim.ant && sim.grid && sim.colors);

	draw_grid_full(sim.grid);

	last = clock();
	sim.is_running = TRUE;

	while (sim.is_running) {
		oldp = sim.ant->pos;
		ant_move(sim.ant, sim.grid, sim.colors);
		grid_silent_expand(sim.grid);
		float diff = (float)(clock() - last) / CLOCKS_PER_SEC;
		
		if (diff >= time_for_frame) {
			oldp = sim.ant->pos;
			ant_move(sim.ant, sim.grid, sim.colors);
			grid_silent_expand(sim.grid);

			if (is_ant_out_of_bounds(sim.ant, sim.grid)) {
				grid_expand(sim.grid, sim.ant);
					draw_menu();
				draw_grid_full(sim.grid);
					doupdate();
				} else {
				draw_grid_iter(sim.grid, oldp);
					draw_menu();
					doupdate();
			}

			handle_input(sim.ant, sim.grid);
			++sim.steps;
			last = clock();
		}
	}
}

void stop_simulation()
{
	sim.is_running = FALSE;
}
