#ifndef __GAME_H__
#define __GAME_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../define.h"
#include "../util/util.h"

typedef struct point Point;
typedef struct line Line;
typedef struct cell Cell;
typedef struct game Game;
typedef struct point_pair Point_Pair;

typedef enum {HORIZONTAL, VERTICAL} Direction;
typedef enum {UP, RIGHT, DOWN, LEFT} Wall;

struct point {
    int row, column;
};

struct point_pair {
    Point first, second;
};

struct line {
    Point src, dest;
    Direction dir;
};

struct cell {
    int walls[4];
    int owner;
    int wall_cnt;
};

struct game {
    Cell grid[MAX_CELLS][MAX_CELLS];
    int scores[MAX_PLYRS];
    int plyr_cnt, lst_winr, cur_plyr, filled_cells;
};

void init_cell(Cell* c);
void init_game_cells(Game* g);
Game* new_game(int plyr_cnt);
void clear_game(Game* g);

Direction find_dir(Point_Pair* p_pair);
Point_Pair sort_pair(Point_Pair* p_pair);
int is_in_grid(Point p, int n);
Point_Pair find_cells(Game* g, Point_Pair* p_pair, Direction d);
int is_filled(Cell c);
void fill_cell(Cell* c, Wall w);
int fill_cell_pair(Game* g, Point_Pair* c_pair, Direction d);
void fill_grid(Game* g, Point_Pair* p_pair);
void change_current_player(Game* g);
void process_move(Game* g, Point_Pair* p_pair);
int game_is_over(Game* g);

void write_horizon(Game* g, int fd, int row, Wall w);
void write_vert(Game* g, int fd, int row, Wall w);
void write_grid(Game* g);
void write_pair(int fd, Point_Pair* p_pair);
void announce_current_player(Game* g);
void announce_result(Game* g);
#endif
