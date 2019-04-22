#include <curses.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct _LINE {
	wchar_t *chars;
	int x, y, vel, height;
} LINE;

void cleanup(LINE *lines, int numlines) {
	for (int i = 0; i < numlines; i++)
		free(lines[i].chars);
	free(lines);
}

int main(int argc, char **argv) {
	setlocale(LC_ALL, "");
	initscr();

	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Terminal does not support colour.\n");
		return -1;
	}
	start_color();

	init_pair(1, COLOR_WHITE, COLOR_BLACK); // Head chars
	init_pair(2, COLOR_GREEN, COLOR_BLACK); // Trail chars

	srand(time(NULL));
	nodelay(stdscr, TRUE);
	curs_set(0);
	cbreak();
	noecho();

	int width, height;
	getmaxyx(stdscr, height, width);
	height--;

	int numlines = width / 2;
	LINE *lines = (LINE *) calloc(numlines, sizeof(LINE));
	if (lines == NULL) {
		endwin();
		fprintf(stderr, "Not enough memory.\n");
		return -1;
	}

	clock_t start, delta;
	start = clock();
	int fps = CLOCKS_PER_SEC / 10;
	while (getch() != 'q') {
		delta = clock() - start;
		start = clock();

		if (delta < fps) {
			usleep(fps - delta);
		}

		clear();
		for (int i = 0; i < numlines; i++) {
			LINE *line = &lines[i];

			// If line is off-screen, restart it elsewhere
			if (line->chars == NULL) {
				while (line->chars == NULL) {
					int x = rand() % width;
					int flag = 1;
					for (int j = 0; j < numlines; j++) {
						if (&lines[j] == line) continue;
						if (lines[j].x == x) {
							flag = 0;
							break;
						}
					}
					if (flag) { // Set-up new line's values
						line->x = x;
						line->y = 0;
						line->vel = (rand() % 3) + 1;
						line->height = (rand() % (height / 2)) + (height / 2);

						line->chars = (wchar_t *) malloc(sizeof(wchar_t) * height);
						if (line->chars == NULL) {
							endwin();
							fprintf(stderr, "Not enough memory.\n");
							cleanup(lines, numlines);
							return -1;
						}
						for (int j = 0; j < height; j++) {
							if (rand() % 2) {
								int index = 0x3000;
								int a = ((rand() % 6) + 10) << 4; // A-F
								int b = rand() % 16; // 0-F
								index |= a | b; // 0x30(A-F)(0-F) Katakana Unicode block
								line->chars[j] = (wchar_t) index;
							} else {
								line->chars[j] = (wchar_t) (0x30 | (rand() % 10)); // 0-9
							}
						}
					}
				}
			} else { // Advance line then re-draw it
				line->y += line->vel;
				if (line->y >= height + line->height) {
					free(line->chars);
					line->chars = NULL;
					continue;
				}
				int y = line->y;
				for (int j = 0; j < line->height; j++) {
					if (0 <= y && y < height) {
						if (j < 3) {
							attron(COLOR_PAIR(1));
						} else {
							attron(COLOR_PAIR(2));
						}

						mvprintw(y, line->x, "%lc", line->chars[y]);

						if (j < 3) {
							attroff(COLOR_PAIR(1));
						} else {
							attroff(COLOR_PAIR(2));
						}
					}
					y--;
				}
			}
		}
		attron(COLOR_PAIR(1));
		mvaddstr(height, width - 18, "press 'q' to quit");
		attroff(COLOR_PAIR(1));
		refresh();
	}

	endwin();
	cleanup(lines, numlines);
	return 0;
}