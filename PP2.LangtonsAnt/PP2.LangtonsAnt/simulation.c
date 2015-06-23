#include "logic.h"
#include "graphics.h"

#define DRAW_EVERY 100
#define INPUT_DELAY 100

static bool running;

static void handle_input(Ant *ant, Grid *grid)
{
	static int input_delay = 0;
	if (input_delay == 0) {
		if (grid_key_command(grid, ant, wgetch(gridw)) != ERR) { // TODO wgetch refreshes
			draw_grid_full(grid);
		}
		input_delay += INPUT_DELAY;
	} else {
		--input_delay;
	}
}

void run_simulation(Ant *ant, Grid *grid, Colors *colors)
{
	static size_t steps = 0, cnt = DRAW_EVERY-1;
	Vector2i oldp;

	assert(ant && grid && colors);

	draw_grid_full(grid);

	running = TRUE;
	while (running) {
		oldp = ant->pos;
		ant_move(ant, grid, colors);
		grid_silent_expand(grid);

		if (is_ant_out_of_bounds(ant, grid)) {
			grid_expand(grid, ant);
			stgs.is_sparse = is_grid_sparse(grid);
			stgs.size = grid->size;
			draw_menu();
			draw_grid_full(grid);
			doupdate();
		} else {
			draw_grid_iter(grid, oldp);
			if (++cnt == DRAW_EVERY) {
				draw_menu();
				cnt = 0;
			}
			doupdate();
		}

		handle_input(ant, grid);
		++stgs.steps;
	}
}

void stop_simulation(void)
{
	running = FALSE;
}
