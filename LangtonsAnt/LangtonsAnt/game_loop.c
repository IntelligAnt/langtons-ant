#include "graphics.h"

static bool run_loop = TRUE;

static input_t handle_input(void)
{
	input_t ret = INPUT_NO_CHANGE;
	Simulation *sim = stgs.linked_sim;
	int ch = getch();
	if (sim) {
		ret |= grid_key_command(sim->grid, sim->ant, ch); // TODO wgetch refreshes
	}
	ret |= menu_key_command(ch);
	return ret;
}

void game_loop(void)
{
	Simulation *sim = stgs.linked_sim;
	draw_grid_full(sim->grid, sim->ant);
	draw_menu_full();

	while (run_loop) {
		input_t input = handle_input();
		input_t grid_changed = input & INPUT_GRID_CHANGED, menu_changed = input & INPUT_MENU_CHANGED;
		sim = stgs.linked_sim;

		if (is_simulation_running(sim)) {
			Vector2i oldp = sim->ant->pos;
			bool in_bounds = ant_move(sim->ant, sim->grid, sim->colors);
			grid_silent_expand(sim->grid);

			if (in_bounds) {
				napms(LOOP_DELAY / (1 << stgs.speed)); // TODO fixed timestep loop
				if (!grid_changed) {
					draw_grid_iter(sim->grid, sim->ant, oldp);
				}
				if (!menu_changed) {
					draw_menu_iter();
				}
			} else {
				grid_expand(sim->grid, sim->ant);
				if (!grid_changed) {
					draw_grid_full(sim->grid, sim->ant);
				}
				if (!menu_changed) {
					draw_menu_full();
				}
			}

			++(sim->steps);
		}

		if (grid_changed) { // TODO move before the update logic
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
