#include "player.h"

int main(int argc, char* argv[]) {
    int port = atoi(argv[1]);
    Player* p = new_player(atoi(argv[2]), atoi(argv[3]));
    init_player(p, port);
    run_player(p);
}