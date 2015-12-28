#include <stdlib.h>
#include "logic.h"
#include "graphics.h"

Simulation *simulation_new(Colors *colors, size_t init_size)
{
	assert(colors);
	Simulation *sim = malloc(sizeof(Simulation));
	sim->colors = colors;
	sim->grid = grid_new(colors, init_size);
	sim->ant = ant_new(sim->grid, UP);
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

void run_simulation(Simulation *sim)
{
	if (sim) {
		sim->is_running = TRUE;
	}
}

void halt_simulation(Simulation *sim)
{
	if (sim) {
		sim->is_running = FALSE;
	}
}

bool is_simulation_running(Simulation *sim)
{
	return sim && sim->is_running;
}

bool has_simulation_started(Simulation *sim)
{
	return sim && sim->steps > 0;
}
