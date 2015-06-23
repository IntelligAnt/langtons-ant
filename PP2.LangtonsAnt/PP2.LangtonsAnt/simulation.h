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

extern Simulation sim;

/* simulation.c */

void run_simulation();
void stop_simulation();

#endif
