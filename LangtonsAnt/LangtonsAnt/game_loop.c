#include "graphics.h"

static bool run_loop = TRUE;

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
			Vector2i oldp = sim->ant->pos;
			if (simulation_step(sim)) {
				napms(LOOP_DELAY / (1 << stgs.speed)); // TODO fixed timestep loop
				draw_grid_iter(sim->grid, sim->ant, oldp);
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
