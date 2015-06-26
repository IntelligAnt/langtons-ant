#include "graphics.h"

int grid_key_command(Grid *grid, Ant *ant, int key)
{
	Vector2i center = { grid->size/2, grid->size/2 };
	Vector2i pos = abs2rel(ant->pos, center);
	Vector2i tl = abs2rel(grid->top_left, center), br = abs2rel(grid->bottom_right, center);
	int o = GRID_VIEW_SIZE / 2 - 1;

	switch (key) {
		/* Arrow keys */
	case KEY_UP:
		scroll_grid(grid, -SCROLL_STEP_MEDIUM, 0);
		break;
	case KEY_DOWN:
		scroll_grid(grid,  SCROLL_STEP_MEDIUM, 0);
		break;
	case KEY_LEFT:
		scroll_grid(grid, 0, -SCROLL_STEP_MEDIUM);
		break;
	case KEY_RIGHT:
		scroll_grid(grid, 0,  SCROLL_STEP_MEDIUM);
		break;

		/* Numpad keys */
	case '8':
		scroll_grid(grid, -SCROLL_STEP_LARGE, 0);
		break;
	case '2':
		scroll_grid(grid,  SCROLL_STEP_LARGE, 0);
		break;
	case '4':
		scroll_grid(grid, 0, -SCROLL_STEP_LARGE);
		break;
	case '6':
		scroll_grid(grid, 0,  SCROLL_STEP_LARGE);
		break;
	case '7':
		scroll_grid(grid, -SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '9':
		scroll_grid(grid, -SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;
	case '1':
		scroll_grid(grid,  SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '3':
		scroll_grid(grid,  SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;

		/* PgUp, PgDn, Home, End */
	case KEY_PPAGE:
		set_scroll(grid, tl.y+o, gridscrl.x);
		break;
	case KEY_NPAGE:
		set_scroll(grid, br.y-o, gridscrl.x);
		break;
	case KEY_HOME:
		set_scroll(grid, gridscrl.y, tl.x+o);
		break;
	case KEY_END:
		set_scroll(grid, gridscrl.y, br.x-o);
		break;

		/* A - Move to ant */
	case 'A': case 'a':
		set_scroll(grid, pos.y, pos.x);
		break;

		/* S - Move to origin */
	case 'S': case 's': case '5':
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
	step = (event.bstate & BUTTON1_CLICKED) ? SCROLL_STEP_SMALL
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
