#include "graphics.h"

int grid_resolve_key(Grid *grid, int key)
{
	switch (key) {
		/* Arrow keys */
	case KEY_UP:
		scroll_grid(grid, -SCROLL_STEP, 0);
		break;
	case KEY_DOWN:
		scroll_grid(grid,  SCROLL_STEP, 0);
		break;
	case KEY_LEFT:
		scroll_grid(grid, 0, -SCROLL_STEP);
		break;
	case KEY_RIGHT:
		scroll_grid(grid, 0,  SCROLL_STEP);
		break;

		/* PgUp, PgDn, Home, End */
	case KEY_PPAGE:
		scroll_grid(grid, -GRID_SCRL_VIEW_SIZE, 0);
		break;
	case KEY_NPAGE:
		scroll_grid(grid,  GRID_SCRL_VIEW_SIZE, 0);
		break;
	case KEY_HOME:
		scroll_grid(grid, 0, -GRID_SCRL_VIEW_SIZE);
		break;
	case KEY_END:
		scroll_grid(grid, 0,  GRID_SCRL_VIEW_SIZE);
		break;

	default:
		return ERR;
	}
	return key;
}
