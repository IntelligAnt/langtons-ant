#include "graphics.h"

static bool run_loop = TRUE, in_bounds;
static Vector2i old_pos;

static input_t handle_input(Simulation* sim)
{
	input_t ret = INPUT_NO_CHANGE;
	int ch = getch();
	if (sim) {
		ret |= grid_key_command(sim->grid, sim->ant, ch); // TODO wgetch refreshes
	}
	ret |= menu_key_command(ch);
	return ret;
}

void game_step(Simulation *sim)
{
	old_pos = sim->ant->pos;
	in_bounds = ant_move(sim->ant, sim->grid, sim->colors);
	grid_silent_expand(sim->grid);
	if (!in_bounds) {
		grid_expand(sim->grid, sim->ant);
	}
	++(sim->steps);
}

void game_loop(void)
{
	Simulation *sim = stgs.linked_sim;
	draw_grid_full(sim->grid, sim->ant);
	draw_menu_full();

	while (run_loop) {
		input_t input = handle_input(sim);
		input_t grid_changed = input & INPUT_GRID_CHANGED, menu_changed = input & INPUT_MENU_CHANGED;
		sim = stgs.linked_sim;

		if (is_simulation_running(sim)) {
			game_step(sim);

			if (in_bounds) {
				napms(LOOP_DELAY / (1 << stgs.speed)); // TODO fixed timestep loop
				draw_grid_iter(sim->grid, sim->ant, old_pos);
				draw_menu_iter();
			} else {
				grid_changed = menu_changed = TRUE;
			}
		}

		if (grid_changed) {
			draw_grid_full(sim->grid, sim->ant);
		}
		if (menu_changed) {
			draw_menu_full();
		}

		doupdate();
	}
}

void stop_game_loop(void)
{
	run_loop = FALSE;
}
