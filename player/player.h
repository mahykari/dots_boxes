#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#include "../define.h"
#include "../util/util.h"
#include "../string_buf/string_buf.h"
#include "../game/game.h"

typedef enum {CMND, PLYR_ID, ID, P1, P1_ROW, P1_COL, P2, P2_ROW, P2_COL} Token_Index;
typedef struct player Player;

struct player {
    int sockfd, plyr_id;
    struct sockaddr_in plyr_addr;
    String_Buf* str_buf;
    Game* g;
};

Player* new_player(int plyr_id, int plyr_cnt);
void clear_player(Player* p);
void bind_player(Player* p, int port);
void init_player(Player* p, int port);
void init_player_fd_set(Player* p, fd_set* readfds);
int find_player_input_src(Player* p, fd_set* readfds);
int handle_wall(Player* p);
int process_player_input(Player* p, int fd);
int run_player(Player* p);

void announce_player(Player* p);
#endif