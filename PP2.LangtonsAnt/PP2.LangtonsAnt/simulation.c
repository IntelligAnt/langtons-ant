#include "logic.h"
#include "graphics.h"

#define INPUT_DELAY 100
#define FPS 30

static bool running;

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

void run_simulation(Ant *ant, Grid *grid, Colors *colors)
{
	static size_t steps = 0;
	Vector2i oldp;
	clock_t last;
	float time_for_frame = 1.0f / FPS;

	assert(ant && grid && colors);

	draw_grid_full(grid);

	running = TRUE;

	last = clock();

	while (running) {
		float diff = (float)(clock() - last) / CLOCKS_PER_SEC;
		
		if (diff >= time_for_frame) {
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
				draw_menu();
				doupdate();
			}

			handle_input(ant, grid);
			++stgs.steps;
			last = clock();
		}
	}
}

void stop_simulation(void)
{
	running = FALSE;
}

bool is_running(void)
{
	return running;
}
