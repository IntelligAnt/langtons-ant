#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "logic.h"
#include "graphics.h"

typedef struct simulation {
	Colors *colors;
	Grid *grid;
	Ant *ant;
	size_t steps;
	bool is_running;
} Simulation;

/* simulation.c */

Simulation *new_simulation();
void delete_simulation(Simulation *sim);
void run_simulation(Simulation *sim);
void stop_simulation(Simulation *sim);

#endif
