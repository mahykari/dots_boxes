#include "game.h"

int main() {
    Game* g = new_game(2);
    write_grid(g);
    Point_Pair p_pair = {{0, 1}, {0, 2}};
    process_move(g, &p_pair);
    
    p_pair = (Point_Pair) {{1, 1}, {1, 2}};
    process_move(g, &p_pair);
    
    p_pair = (Point_Pair) {{1, 1}, {1, 0}};
    process_move(g, &p_pair);

    p_pair = (Point_Pair) {{0, 1}, {0, 0}};
    process_move(g, &p_pair);
    
    p_pair = (Point_Pair) {{0, 0}, {1, 0}};
    process_move(g, &p_pair);

    p_pair = (Point_Pair) {{0, 2}, {1, 2}};
    process_move(g, &p_pair);

    p_pair = (Point_Pair) {{0, 1}, {1, 1}};
    process_move(g, &p_pair);

    clear_game(g);
}