#include <string.h>
#include <stdlib.h>

#include "server.h"

int main(int argc, char* argv[]) {
    int port = atoi(argv[1]);
    Server* s = new_server();
    init_server(s, port);
    log_msg(STDOUT_FILENO, SERVER, "Server binded. Listening...\n");
    run_server(s);
    clear_server(s);
}