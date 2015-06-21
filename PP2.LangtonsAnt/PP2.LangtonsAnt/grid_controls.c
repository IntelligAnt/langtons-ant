#include "graphics.h"

int grid_key_command(Grid *grid, Ant *ant, int key)
{
	int mid = grid->size / 2;
	Vector2i rel;

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

		/* A - Move to ant */
	case 'A': case 'a':
		rel = abs2rel(ant->pos, (Vector2i) { mid, mid });
		set_scroll(grid, rel.y, rel.x);
		break;

		/* S - Move to origin */
	case 'S': case 's':
		set_scroll(grid, 0, 0);
		break;

	default:
		return ERR;
	}
	return key;
}
