#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "../string_buf/string_buf.h"
#include "../util/util.h"
#include "../player/player.h"

typedef struct client Client;

struct client {
    int sockfd;
    struct sockaddr_in serv_addr;
    String_Buf* str_buf;
    Player* p;
};

Client* new_client();
void connect_to_server(Client* c, int port);
void init_client(Client* c, int port);
void clear_client(Client* c);
void talk_to_server(Client* c);
int find_input_src(Client* c, fd_set* readfds);
int process_input(Client* c, int fd);
int process_command(Client* c);
int process_msg(Client* c);

#endif