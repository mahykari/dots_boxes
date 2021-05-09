#include "game.h"

void init_cell(Cell* c) {
    *c = (Cell) {.owner = -1, .wall_cnt = 0};
    memset(c->walls, 0, 4 * sizeof(int));
}

void init_game_cells(Game* g) {
    for (int i = 0; i < MAX_CELLS; i++) {
        for (int j = 0; j < MAX_CELLS; j++) {
            init_cell(&g->grid[i][j]);
        }
    }
}

Game* new_game(int plyr_cnt) {
    Game* g = (Game*)malloc(sizeof(Game));
    *g = (Game) {.plyr_cnt = plyr_cnt, .lst_winr = -1, .cur_plyr = 0, .filled_cells = 0};
    memset(g->scores, 0, MAX_PLYRS * sizeof(int));
    init_game_cells(g);
    return g;
}

void clear_game(Game* g) {
    write_msg(STDOUT_FILENO, "Clearing game...\n");
    free(g);
}

Direction find_dir(Point_Pair* p_pair) {
    Direction res;
    if (p_pair->first.row == p_pair->second.row)
        res = HORIZONTAL;
    if (p_pair->first.column == p_pair->second.column)
        res = VERTICAL;
    return res;
}

Point_Pair sort_pair(Point_Pair* p_pair) {
    Point_Pair res;
    Direction d = find_dir(p_pair);
    if (d == HORIZONTAL) {
        int r = p_pair->first.row;
        int min_c = min(p_pair->first.column, p_pair->second.column);
        int max_c = max(p_pair->first.column, p_pair->second.column);
        res = (Point_Pair) {{r, min_c}, {r, max_c}};
    }

    if (d == VERTICAL) {
        int c = p_pair->first.column;
        int min_r = min(p_pair->first.row, p_pair->second.row);
        int max_r = max(p_pair->first.row, p_pair->second.row);
        res = (Point_Pair) {{min_r, c}, {max_r, c}};
    }

    return res;
}

int is_in_grid(Point p, int n) {
    if (p.row < 0 || p.row > n - 1)
        return 0;
    if (p.column < 0 || p.column > n - 1)
        return 0;
    
    return 1;
}

Point_Pair find_cells(Game* g, Point_Pair* p_pair, Direction d) {
    Point_Pair res = {{-1, -1}, {-1, -1}};
    if (d == HORIZONTAL) {
        int c = p_pair->first.column;
        int lower_r = p_pair->first.row;
        int higher_r = lower_r - 1;

        if (is_in_grid((Point) {lower_r, c}, g->plyr_cnt))
            res.second = (Point) {lower_r, c};
        if (is_in_grid((Point) {higher_r, c}, g->plyr_cnt))
            res.first = (Point) {higher_r, c};
    }

    if (d == VERTICAL) {
        int r = p_pair->first.row;
        int right_c = p_pair->first.column;
        int left_c = right_c - 1;

        if (is_in_grid((Point) {r, right_c}, g->plyr_cnt))
            res.second = (Point) {r, right_c};
        if (is_in_grid((Point) {r, left_c}, g->plyr_cnt))
            res.first = (Point) {r, left_c};
    }
    
    return res;
}

void fill_cell(Cell* c, Wall w) {
    if (c->walls[w] != 1) {
        c->walls[w] = 1;
        c->wall_cnt += 1;
    }
}

int is_filled(Cell c) { return c.wall_cnt == 4; }

int fill_cell_pair(Game* g, Point_Pair* c_pair, Direction d) {
    Point f = c_pair->first, s = c_pair->second;
    if (d == HORIZONTAL) {
        if (is_in_grid(f, g->plyr_cnt))
            fill_cell(&g->grid[f.row][f.column], DOWN);
        fill_cell(&g->grid[s.row][s.column], UP);
    }

    if (d == VERTICAL) {
        if (is_in_grid(f, g->plyr_cnt))
            fill_cell(&g->grid[f.row][f.column], RIGHT);
        fill_cell(&g->grid[s.row][s.column], LEFT);
    }

    int filled_cells = 0;
    if (is_in_grid(f, g->plyr_cnt)) {
        if (is_filled(g->grid[f.row][f.column])) {
            g->grid[f.row][f.column].owner = g->cur_plyr;
            filled_cells += 1;
        }
    }

    if (is_filled(g->grid[s.row][s.column])) {
        g->grid[s.row][s.column].owner = g->cur_plyr;
        filled_cells += 1;
    }

    return filled_cells;
}

void fill_grid(Game* g, Point_Pair* p_pair) {
    Point_Pair sorted_pair = sort_pair(p_pair);
    Direction d = find_dir(&sorted_pair);
    Point_Pair cells = find_cells(g, &sorted_pair, d);
    int res = fill_cell_pair(g, &cells, d);
    g->scores[g->cur_plyr] += res;
    g->filled_cells += res;
    if (res)
        g->lst_winr = g->cur_plyr;
}

void change_current_player(Game* g) {
    if (g->lst_winr != -1)
        g->cur_plyr = g->lst_winr;
    else
        g->cur_plyr = (g->cur_plyr + 1) % g->plyr_cnt;
    
    g->lst_winr = -1;
    announce_current_player(g);
}

void process_move(Game* g, Point_Pair* p_pair) {
    fill_grid(g, p_pair);
    write_grid(g);
    change_current_player(g);
}

int game_is_over(Game* g) { return g->filled_cells == g->plyr_cnt * g->plyr_cnt; }

void write_horizon(Game* g, int fd, int row, Wall w) {
    for (int j = 0; j < g->plyr_cnt; j++) {
        write_char(fd, '+', 1);
        if (g->grid[row][j].walls[w] == 1)
            write_char(fd, '-', 3);

        else
            write_char(fd, ' ', 3);
    }

    write_char(fd, '+', 1);
}

void write_vert(Game* g, int fd, int row, Wall w) {
    for (int j = 0; j < g->plyr_cnt; j++) {
        if (g->grid[row][j].walls[w] == 1)
            write_char(fd, '|', 1);

        if (g->grid[row][j].walls[w] == 0)
            write_char(fd, ' ', 1);
        
        write_char(fd, ' ', 1);
        
        if (g->grid[row][j].owner == -1)
            write_char(fd, ' ', 1);

        if (g->grid[row][j].owner != -1)
            write_char(fd, '0' + g->grid[row][j].owner, 1);

        write_char(fd, ' ', 1);
    }
}

void write_grid(Game* g) {
    write_char(STDOUT_FILENO, '\n', 1);
    write_char(STDOUT_FILENO, '-', 5 * g->plyr_cnt);
    write_char(STDOUT_FILENO, '\n', 1);

    for (int i = 0; i < g->plyr_cnt; i++) {
        write_horizon(g, STDOUT_FILENO, i, UP);
        write_char(STDOUT_FILENO, '\n', 1);
        write_vert(g, STDOUT_FILENO, i, LEFT);
        if (g->grid[i][g->plyr_cnt - 1].walls[RIGHT] == 0)
            write_char(STDOUT_FILENO, ' ', 1);
        else
            write_char(STDOUT_FILENO, '|', 1);
        
        write_char(STDOUT_FILENO, '\n', 1);
    }


    write_horizon(g, STDOUT_FILENO, g->plyr_cnt - 1, DOWN);
    
    write_char(STDOUT_FILENO,'\n', 1);
    write_char(STDOUT_FILENO, '-', 5 * g->plyr_cnt);
    write_char(STDOUT_FILENO, '\n', 2);
}

void write_pair(int fd, Point_Pair* p_pair) {
    char buf[16];
    memset(buf, 0, 16);
    snprintf(buf, 16, "((");
    write_msg(fd, buf);
    snprintf(buf, 16, "%d, %d)", p_pair->first.row, p_pair->first.column);
    write_msg(fd, buf);
    snprintf(buf, 16, ", (%d, %d))", p_pair->second.row, p_pair->second.column);
    write_msg(fd, buf);
}

void announce_current_player(Game* g) {
    char buf[BUF_SZ];
    memset(buf, 0, BUF_SZ);
    snprintf(buf, BUF_SZ, "Current player changed to %d\n", g->cur_plyr);
    log_msg(STDOUT_FILENO, GAME, buf);
}

void announce_result(Game* g) {
    char buf[BUF_SZ];
    write_dashes(STDOUT_FILENO, 32);
    log_msg(STDOUT_FILENO, GAME, GAME_RES);
    for (int i = 0; i < g->plyr_cnt; i++) {
        memset(buf, 0, BUF_SZ);
        snprintf(buf, BUF_SZ, "Player %d: %d\n", i, g->scores[i]);
        write_msg(STDOUT_FILENO, buf);
    }
    
    write_dashes(STDOUT_FILENO, 32);
}