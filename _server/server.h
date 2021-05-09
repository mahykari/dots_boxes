#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../define.h"
#include "../util/util.h"
#include "../string_buf/string_buf.h"
#include "../matchmaker/matchmaker.h"

typedef struct server Server;

struct server {
    int sockfd;
    struct sockaddr_in serv_addr;
    int clntfd_arr[MAX_PLYRS];
    String_Buf* str_buf;
    Matchmaker* mtch_mk;
};

Server* new_server();

void bind_server(Server* s, int port);
void start_listening(Server* s);

void init_server(Server* s, int port);
void clear_server(Server* s);
void run_server(Server* s);

int init_server_fd_set(Server* s, fd_set* readfds);
int find_server_input_src(Server* s, fd_set* readfds);

void handle_new_client(Server* s);
void handle_disconnected_client(Server* s, int fd);
void handle_match_size(Server* s, int fd, int match_sz);

void add_new_client(Server* s, int clntfd);
int add_clients_to_set(Server* s, fd_set* readfds);
void process_server_input(Server* s, int fd);
void process_server_command(Server* s, int fd);

void announce_new_client(Server* s, int clntfd, struct sockaddr_in* clnt_addr);

#endif