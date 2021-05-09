#ifndef __MATCHMAKER_H__
#define __MATCHMAKER_H__

#include "../define.h"
#include "../util/util.h"

typedef struct match Match;
typedef struct matchmaker Matchmaker;

struct match {
    int* player_fds;
    int match_sz;
    int player_cnt;
    char buf[BUF_SZ];
};

struct matchmaker {
    Match matches[MTCH_SIZE_CNT];
    char buf[BUF_SZ];
    int port_id;
};

Match new_match(int match_sz);
Matchmaker* new_matchmaker();
void clear_matchmaker(Matchmaker* mtch_mk);

void add_player_to_match(Matchmaker* mtch_mk, int fd, int match_sz);
void add_player(Match* m, int fd);
int check_for_ready_match(Match* m);
void announce_match_to_clients(Match* m, int port);

#endif