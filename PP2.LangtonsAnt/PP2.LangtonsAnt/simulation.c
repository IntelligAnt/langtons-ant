#include <stdlib.h>
#include "logic.h"
#include "graphics.h"

Simulation *simulation_new(Colors *colors, size_t init_size)
{
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
	sim->is_running = TRUE;
}

void stop_simulation(Simulation *sim)
{
	sim->is_running = FALSE;
}

bool is_simulation_valid(Simulation *sim)
{
	return (sim && sim->colors && sim->grid && sim->ant);
}
