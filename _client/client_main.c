#include <string.h>
#include <stdlib.h>

#include "client.h"

int main(int argc, char* argv[]) {
    int port = atoi(argv[1]);
    Client* c = new_client();
    init_client(c, port);
    talk_to_server(c);
    run_player(c->p);
    clear_client(c);
}