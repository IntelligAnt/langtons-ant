#include <stdlib.h>
#include "logic.h"

Ant *ant_new(Grid *grid, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->pos.y = ant->pos.x = grid->size / 2;
	ant->dir = dir;
	return ant;
}

void ant_delete(Ant *ant)
{
	free(ant);
}

static void change_dir(Ant *ant, int turn)
{
	switch (ant->dir) {
	case DIR_UP:
		ant->pos.x += turn;
		break;
	case DIR_RIGHT:
		ant->pos.y += turn;
		break;
	case DIR_DOWN:
		ant->pos.x -= turn;
		break;
	case DIR_LEFT:
		ant->pos.y -= turn;
		break;
	}
	ant->dir = (ant->dir + turn + 4) % 4;
}

static void update_bounding_box(Grid *grid, Vector2i pos)
{
	Vector2i *tl = &grid->top_left, *br = &grid->bottom_right;
	tl->y = min(tl->y, pos.y);
	tl->x = min(tl->x, pos.x);
	br->y = max(br->y, pos.y);
	br->x = max(br->x, pos.x);
}

static bool is_grid_usage_low(Grid *grid)
{
	int b = (grid->bottom_right.y - grid->top_left.y + 1)*(grid->bottom_right.x - grid->top_left.x + 1);
	return (double)grid->colored / b < GRID_USAGE_THRESHOLD;
}

static void ant_move_n(Ant *ant, Grid *grid, Colors *colors)
{
	byte *c = &grid->c[ant->pos.y][ant->pos.x];
	bool is_def = *c == colors->def;
	short turn;

	if (is_def) {
		grid->colored++;
		update_bounding_box(grid, ant->pos);
	}

	// In-place color changing
	if (is_color_special(colors, *c)) {
		*c = (byte)colors->next[*c];
	}

	turn = colors->turn[*c];
	assert(abs(turn) == 1);
	*c = (byte)colors->next[*c];
	change_dir(ant, turn);

	if (is_def && IS_GRID_LARGE(grid) && is_grid_usage_low(grid)) {
		grid_make_sparse(grid);
	}
}

static void ant_move_s(Ant *ant, Grid *grid, Colors *colors)
{
	int y = ant->pos.y, x = ant->pos.x, turn;
	Cell **t = grid->csr + y;

	while (*t && CELL_GET_COLUMN(*t) < (size_t)x) {
		t = &(*t)->next;
	}
	if (!*t || CELL_GET_COLUMN(*t) != (size_t)x) {
		if (!*t) {
			grid->colored++;
			update_bounding_box(grid, ant->pos);
		}
		new_cell(t, x, (byte)colors->first);
	}

	// In-place color changing
	if (is_color_special(colors, CELL_GET_COLOR(*t))) {
		CELL_SET_COLOR(*t, colors->next[CELL_GET_COLOR(*t)]);
	}

	turn = colors->turn[CELL_GET_COLOR(*t)];
	assert(abs(turn) == 1);
	CELL_SET_COLOR(*t, (byte)colors->next[CELL_GET_COLOR(*t)]);
	change_dir(ant, turn);
}

bool ant_move(Ant *ant, Grid *grid, Colors *colors) // TODO have ant_move return move direction (either relative or absolute)
{
	if (is_grid_sparse(grid)) {
		ant_move_s(ant, grid, colors);
	} else {
		ant_move_n(ant, grid, colors);
	}
	return !is_ant_out_of_bounds(ant, grid);
}

bool is_ant_out_of_bounds(Ant *ant, Grid *grid)
{
	return ant->pos.y < 0 || (size_t)ant->pos.y >= grid->size
		|| ant->pos.x < 0 || (size_t)ant->pos.x >= grid->size;
}
