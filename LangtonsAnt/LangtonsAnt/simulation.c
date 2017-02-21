#include <stdlib.h>

#include "logic.h"

Simulation *simulation_new(Colors *colors, size_t init_size)
{
	assert(colors);
	Simulation *sim = malloc(sizeof(Simulation));
	sim->colors = colors;
	sim->grid = grid_new(colors, init_size);
	sim->ant = ant_new(sim->grid, DIR_UP);
	sim->steps = 0;
	sim->is_running = FALSE;
	return sim;
}

void simulation_delete(Simulation *sim)
{
	grid_delete(sim->grid);
	ant_delete(sim->ant);
	free(sim);
}

void simulation_run(Simulation *sim)
{
	sim->is_running = TRUE;
}

void simulation_halt(Simulation *sim)
{
	sim->is_running = FALSE;
}

bool simulation_step(Simulation *sim)
{
	bool in_bounds = ant_move(sim->ant, sim->grid, sim->colors);
	++(sim->steps);
	grid_silent_expand(sim->grid);
	if (!in_bounds) {
		grid_expand(sim->grid, sim->ant);
	}
	return in_bounds;
}

bool is_simulation_running(Simulation *sim)
{
	return sim && sim->is_running;
}

bool has_simulation_started(Simulation *sim)
{
	return sim && sim->steps > 0;
}
