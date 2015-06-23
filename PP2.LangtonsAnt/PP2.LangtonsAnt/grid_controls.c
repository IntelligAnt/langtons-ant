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
		scroll_grid(grid, -SCROLL_STEP_BIG, 0);
		break;
	case KEY_NPAGE:
		scroll_grid(grid,  SCROLL_STEP_BIG, 0);
		break;
	case KEY_HOME:
		scroll_grid(grid, 0, -SCROLL_STEP_BIG);
		break;
	case KEY_END:
		scroll_grid(grid, 0,  SCROLL_STEP_BIG);
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

	case KEY_MOUSE:
		grid_mouse_command(grid);
		break;

	default:
		return ERR;
	}
	return key;
}

typedef enum { SB_VERTICAL, SB_HORIZONTAL } ScrollbarType;

static void scrollbar_clicked(Grid *grid, MEVENT event, ScrollbarType sbtype)
{
	int n = GRID_VIEW_SIZE, mid = n/2, step = n-2;
	Vector2i pos = { event.y, event.x }, rel;
	
	if (sbtype == SB_VERTICAL) {
		rel = abs2rel(pos, (Vector2i) { mid, GRID_VIEW_SIZE });
	} else {
		rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid });
	}

	if (event.bstate & BUTTON1_CLICKED) {
		rel.y -= gridscrl.vcenter, rel.x -= gridscrl.hcenter;
		scroll_grid(grid, sgn(rel.y)*step, sgn(rel.x)*step);
	} else if (event.bstate & BUTTON3_CLICKED) {
		if (sbtype == SB_VERTICAL) {
			set_scroll(grid, (int)(rel.y/gridscrl.scale), gridscrl.x);
		} else {
			set_scroll(grid, gridscrl.y, (int)(rel.x/gridscrl.scale));
		}
	}
}

void grid_mouse_command(Grid *grid)
{
	MEVENT event;
	int step;
	
	nc_getmouse(&event);
	step = (event.bstate & BUTTON1_CLICKED) ? SCROLL_STEP
		 : (event.bstate & BUTTON3_CLICKED) ? grid->size // Can be anything large
		 : 0;

	/* Vertical scrollbar */
	if (event.x == GRID_VIEW_SIZE && event.y < GRID_VIEW_SIZE) {
		if (event.y == 0) {
			scroll_grid(grid, -step, 0);
		} else if (event.y == GRID_VIEW_SIZE-1) {
			scroll_grid(grid,  step, 0);
		} else {
			scrollbar_clicked(grid, event, SB_VERTICAL);
		}
	/* Horizontal scrollbar */
	} else if (event.y == GRID_VIEW_SIZE && event.x < GRID_VIEW_SIZE) {
		if (event.x == 0) {
			scroll_grid(grid, 0, -step);
		} else if (event.x == GRID_VIEW_SIZE-1) {
			scroll_grid(grid, 0,  step);
		} else {
			scrollbar_clicked(grid, event, SB_HORIZONTAL);
		}
	}
}
