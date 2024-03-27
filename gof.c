#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#define BOARD_WIDTH  50
#define BOARD_HEIGHT 20

#define ALIVE '#'
#define DEAD  '.'

int clamp(int x, int s, int l) {
	if (x < s) return s;
	if (x > l) return l;
	return x;
}

void delay(int ms) {
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

void clear_screen() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

typedef struct {
	char front_buff[BOARD_HEIGHT][BOARD_WIDTH + 1]; // + 1 for the '\0' character
	char back_buff[BOARD_HEIGHT][BOARD_WIDTH + 1];
} Board;


/*
 * Loads the board from a text file.
 */

void load_board(Board* board, const char* filename) {
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		printf("Failed to open file: %s\n", filename);
		exit(1);
	}

	for (int i = 0; i < BOARD_HEIGHT; i++) {
		char buffer[BOARD_WIDTH + 1];
		fread(buffer, BOARD_WIDTH, 1, f);
		buffer[BOARD_WIDTH] = '\0';

#ifdef _WIN32
		fseek(f, 2, SEEK_CUR);
#else
		fseek(f, 1, SEEK_CUR);
#endif

		strcpy(board->front_buff[i], buffer);
		strcpy(board->back_buff[i], buffer);
	}

	fclose(f);
}

/*
 * Dumps the board into the console
 */

void print_board(Board* board) {
	// Copying the back buffer to the front buffer
	memcpy(board->front_buff, board->back_buff, BOARD_WIDTH * BOARD_HEIGHT);

//	printf("         ┏━━━┓╋╋╋╋╋╋╋╋╋╋╋╋╋╋┏━┓╋┏┓╋╋┏━┓\n");
//	printf("         ┃┏━┓┃╋╋╋╋╋╋╋╋╋╋╋╋╋╋┃┏┛╋┃┃╋╋┃┏┛\n");
//	printf("         ┃┃╋┗╋━━┳┓┏┳━━┓╋┏━━┳┛┗┓╋┃┃┏┳┛┗┳━━┓\n");
//	printf("         ┃┃┏━┫┏┓┃┗┛┃┃━┫╋┃┏┓┣┓┏┛╋┃┃┣╋┓┏┫┃━┫\n");
//	printf("         ┃┗┻━┃┏┓┃┃┃┃┃━┫╋┃┗┛┃┃┃╋╋┃┗┫┃┃┃┃┃━┫\n");
//	printf("         ┗━━━┻┛┗┻┻┻┻━━┛╋┗━━┛┗┛╋╋┗━┻┛┗┛┗━━┛\n\n");

	printf("+");
	for (int i = 0; i < BOARD_WIDTH; i++) {
		printf("-");
	}
	printf("+\n");

	for (int i = 0; i < BOARD_HEIGHT; i++) {
		printf("|");
		for (int j = 0; j < BOARD_WIDTH; j++) {
			printf("%c", board->front_buff[i][j]);
		}
		printf("|\n");
	}

	printf("+");
	for (int i = 0; i < BOARD_WIDTH; i++) {
		printf("-");
	}
	printf("+\n");
}

/*
 * Function that returns the number of alive neighbour
 * for a given cell position.
 */

int get_alive_neighbours(Board* board, int x, int y) {
	int alive = 0;

	int x_min = clamp(x - 1, 0, BOARD_WIDTH);
	int x_max = clamp(x + 1, 0, BOARD_WIDTH);
	int y_min = clamp(y - 1, 0, BOARD_HEIGHT);
	int y_max = clamp(y + 1, 0, BOARD_HEIGHT);

	for (int i = y_min; i <= y_max; i++) {
		for (int j = x_min; j <= x_max; j++) {
			if (i != y || j != x) {
				if (board->front_buff[i][j] == ALIVE) alive++;
			}
		}
	}
	return alive;
}

/*
 * Forwards the simulation steps
 */

void forward(Board* board) {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			char cell = board->front_buff[i][j];
			int alive = get_alive_neighbours(board, j, i);

			if (cell == ALIVE) {
				if (alive < 2)                     board->back_buff[i][j] = DEAD;
				else if (alive == 2 || alive == 3) board->back_buff[i][j] = ALIVE;
				else if (alive > 3)                board->back_buff[i][j] = DEAD;
			} else if (cell == DEAD) {
				if (alive == 3)                    board->back_buff[i][j] = ALIVE;
			}
		}
	}
}

int main() {
	Board board;
	load_board(&board, "board.txt");

	while (1) {
		clear_screen();
		print_board(&board);
		forward(&board);
		delay(500);
	}
	return 0;
}
