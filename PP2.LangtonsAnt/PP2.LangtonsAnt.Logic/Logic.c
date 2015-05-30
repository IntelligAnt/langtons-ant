#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#include <Windows.h>

#define SIZE 20
#define CENTER_X SIZE/2
#define CENTER_Y SIZE/2

#define DRAW_EVERY 1
//#define DELAY 0

#define CLEAR() system("cls");
//#define CLEAR() printf("\033[0;0H\n");

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;

char **grid_new()
{
	char **grid = malloc(SIZE * sizeof(char*)), def_color = 15;
	int i, j;
	for (i = 0; i < SIZE; ++i) {
		grid[i] = malloc(SIZE * sizeof(char));
		for (j = 0; j < SIZE; ++j) {
			grid[i][j] = def_color;
		}
	}

	return grid;
}

int color_ex(short* colors, short c) {
	if (colors[c] != 0) return 1;
	else return 0;
}

int new_color(short* colors, short c, short turn, int* numb) /*sluzi i za menjanje pravila te boje*/
{
	if (!color_ex(colors, c)) (*numb)++;
	colors[c] = turn;
}

int delete_color(short* colors, short c, int* numb)
{
	if (color_ex(colors,c)) (*numb)--;
	colors[c] = 0;
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


int ant_move(Ant *ant, char **grid, short* colors)
{
	int x = ant->x, y = ant->y;
	int turn = colors[grid[x][y]], i=1;
	grid[x][y] = grid[x][y] == 15 ? 0 : grid[x][y] + 1;
	while (colors[grid[x][y]] == 0)
		grid[x][y] = grid[x][y] == 15 ? 0 : grid[x][y] + 1;

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

	ant->dir = (ant->dir + turn + 4) % 4;
}

int draw_grid(char **grid, int steps)
{
	int i, j;

	CLEAR();

	for (i = 0; i < SIZE; ++i) {
		for (j = 0; j < SIZE; ++j) {
			putchar(grid[i][j]);
		}
		putchar('\n');
	}

	printf("Steps: %d\n", steps);
}

int enough_colors(int numb) {
	if (numb < 2) return 0;
	else return 1;
}

int main(void)
{
	Ant *ant = ant_new(CENTER_X, CENTER_Y, UP);
	short color_list[16];
	char **grid = grid_new();
	int i, steps = 0, *color_numb = malloc(sizeof(int)), cnt = DRAW_EVERY - 1;
	*color_numb = 0;
	for (i = 0; i < 16; color_list[i++] = 0);

	i = 1;
	short color, turn;
	while (i) {
		printf("1. Nova boja.\n2. Izbrisi boju.\n0 za crtanje\n");
		scanf("%d", &i);
		switch (i) {
		case 0: if (!enough_colors(*color_numb)) i = 1; break;
		case 1: printf("Unesite boju (0-15) i smer skretanja (-1 ili 1)\n");
			scanf("%hi %hi", &color, &turn);
			assert(abs(turn) == 1);
			new_color(color_list, color, turn, color_numb);
			break;
		case 2: printf("Unesite boju (0-15)\n");
			scanf("%hi", &color);
			delete_color(color_list, color, color_numb);
			break;
		}
		CLEAR();
	}

	while (ant->x >= 0 && ant->x < SIZE && ant->y >= 0 && ant->y < SIZE) {
		if (++cnt == DRAW_EVERY) {
			draw_grid(grid, steps);
			cnt = 0;
		}
		ant_move(ant, grid, color_list);
		++steps;
		//Sleep(DELAY);
	}

	return 0;
}
