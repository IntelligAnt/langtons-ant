#include "graphics.h"

input_t grid_key_command(Grid *grid, Ant *ant, int key)
{
	Vector2i center = { grid->size/2, grid->size/2 };
	Vector2i pos = abs2rel(ant->pos, center);
	Vector2i tl = abs2rel(grid->top_left, center), br = abs2rel(grid->bottom_right, center);
	int o = GRID_VIEW_SIZE / 2 - 1;

	switch (key) {
		/* Scroll - arrow keys */
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

		/* Scroll - numpad keys */
	case '8': case KEY_A2:
		scroll_grid(grid, -SCROLL_STEP_LARGE, 0);
		break;
	case '2': case KEY_C2:
		scroll_grid(grid,  SCROLL_STEP_LARGE, 0);
		break;
	case '4': case KEY_B1:
		scroll_grid(grid, 0, -SCROLL_STEP_LARGE);
		break;
	case '6': case KEY_B3:
		scroll_grid(grid, 0,  SCROLL_STEP_LARGE);
		break;
	case '7': case KEY_A1:
		scroll_grid(grid, -SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '9': case KEY_A3:
		scroll_grid(grid, -SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;
	case '1': case KEY_C1:
		scroll_grid(grid,  SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '3': case KEY_C3:
		scroll_grid(grid,  SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;

		/* Jump to bounds */
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

		/* Jump to ant */
	case 'F': case 'f': case '5': case KEY_B2:
		set_scroll(grid, pos.y, pos.x);
		break;

		/* Jump to center */
	case 'C': case 'c': case '0': case PAD0:
		set_scroll(grid, 0, 0);
		break;

	case KEY_MOUSE:
		return grid_mouse_command(grid);

	default:
		return INPUT_NO_CHANGE;
	}

	return INPUT_GRID_CHANGED;
}

typedef enum { SB_VERTICAL, SB_HORIZONTAL } ScrollbarType;

static void scrollbar_clicked(Grid *grid, MEVENT event, ScrollbarType sbtype)
{
	int n = GRID_VIEW_SIZE, mid = n/2, step = n-2;
	Vector2i pos = { event.y, event.x }, rel;

	if (event.bstate & BUTTON1_CLICKED) {
		if (sbtype == SB_VERTICAL) {
			rel = abs2rel(pos, (Vector2i) { mid+gridscrl.vcenter, GRID_VIEW_SIZE });
		} else {
			rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid+gridscrl.hcenter });
		}
		scroll_grid(grid, sgn(rel.y)*step, sgn(rel.x)*step);
	} else if (event.bstate & BUTTON3_CLICKED) {
		if (sbtype == SB_VERTICAL) {
			rel = abs2rel(pos, (Vector2i) { mid, GRID_VIEW_SIZE });
			set_scroll(grid, (int)(rel.y/gridscrl.scale), gridscrl.x);
		} else {
			rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid });
			set_scroll(grid, gridscrl.y, (int)(rel.x/gridscrl.scale));
		}
	}
}

input_t grid_mouse_command(Grid *grid)
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
		return INPUT_GRID_CHANGED;
	/* Horizontal scrollbar */
	} else if (event.y == GRID_VIEW_SIZE && event.x < GRID_VIEW_SIZE) {
		if (event.x == 0) {
			scroll_grid(grid, 0, -step);
		} else if (event.x == GRID_VIEW_SIZE-1) {
			scroll_grid(grid, 0,  step);
		} else {
			scrollbar_clicked(grid, event, SB_HORIZONTAL);
		}
		return INPUT_GRID_CHANGED;
	}

	return INPUT_NO_CHANGE;
}
