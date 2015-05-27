#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#include <Windows.h>

#define SIZE 20
#define CENTER_X SIZE/2
#define CENTER_Y SIZE/2
#define COLOR_COUNT 4

#define DRAW_EVERY 1
//#define DELAY 0

#define CLEAR() system("cls");
//#define CLEAR() printf("\033[0;0H\n");

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

typedef struct color {
	char c;
	int turn;
	struct color *next;
} Color;

Color **grid_new(Color def_color)
{
	Color **grid = malloc(SIZE * sizeof(Color*));
	int i, j;

	def_color.c = ' ';

	for (i = 0; i < SIZE; ++i) {
		grid[i] = malloc(SIZE * sizeof(Color));
		for (j = 0; j < SIZE; ++j) {
			grid[i][j] = def_color;
		}
	}

	return grid;
}

Color *get_colors(void)
{
	Color *color, *first = NULL, *last = NULL;
	char c;
	int turn;

	printf("Uneti boje u formatu: znak pravac\n");
	while (scanf("%c", &c) && c != '\n' && scanf("%d", &turn) && abs(turn) == 1) {
		color = malloc(sizeof(Color));
		color->c = c;
		color->turn = turn;
		if (!first) {
			first = color;
		} else {
			last->next = color;
		}
		last = color;
		fflush(stdin);
	}
	last->next = first;

	return first;
}

typedef struct ant {
	int x, y;
	Direction dir;
} Ant;

Ant *ant_new(int x, int y, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->x = x;
	ant->y = y;
	ant->dir = dir;
	return ant;
}

int cycle_colors(Color *pcolor)
{
	int turn = pcolor->turn;
	*pcolor = *pcolor->next;
	return turn;
}

void ant_move(Ant *ant, Color **grid)
{
	int x = ant->x, y = ant->y;
	int turn = cycle_colors(&grid[x][y]);
	assert(abs(turn) == 1);

	switch (ant->dir) {
	case UP:
		ant->y += turn;
		break;
	case RIGHT:
		ant->x += turn;
		break;
	case DOWN:
		ant->y -= turn;
		break;
	case LEFT:
		ant->x -= turn;
		break;
	}

	ant->dir = (ant->dir + turn + COLOR_COUNT) % COLOR_COUNT;
}

void draw_grid(Color **grid, int steps)
{
	int i, j;

	CLEAR();

	for (i = 0; i < SIZE; ++i) {
		for (j = 0; j < SIZE; ++j) {
			putchar(grid[i][j].c);
		}
		putchar('\n');
	}

	printf("Steps: %d\n", steps);
}

int main(void)
{
	Ant *ant = ant_new(CENTER_X, CENTER_Y, UP);
	Color *color_list = get_colors();
	Color **grid = grid_new(*color_list);
	int steps = 0, cnt = DRAW_EVERY-1;

	while (ant->x >= 0 && ant->x < SIZE && ant->y >= 0 && ant->y < SIZE) {
		if (++cnt == DRAW_EVERY) {
			draw_grid(grid, steps);
			cnt = 0;
		}
		ant_move(ant, grid);
		++steps;
		//Sleep(DELAY);
	}

	return 0;
}
