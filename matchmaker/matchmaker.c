#include "matchmaker.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Match new_match(int match_sz) {
    Match m = { .match_sz = match_sz, .player_cnt = 0 };
    m.player_fds = (int*)malloc(match_sz * sizeof(int));
    memset(m.player_fds, 0, match_sz * sizeof(int));
    return m;
}

Matchmaker* new_matchmaker() {
    Matchmaker* m = (Matchmaker*)malloc(sizeof(Matchmaker));
    m->port_id = BASE_PORT_ID + 1;
    memset(m->buf, 0, BUF_SZ);
    for (int i = 0; i < MTCH_SIZE_CNT; i++) {
        m->matches[i] = new_match(i + 2);
    }
    
    return m;
}

void clear_matchmaker(Matchmaker* mtch_mk) {
    free(mtch_mk);
}

void add_player_to_match(Matchmaker* mtch_mk, int fd, int match_sz) {
    Match* m = &mtch_mk->matches[match_sz - 2];
    add_player(m, fd);
    if (check_for_ready_match(m)) {
        snprintf(mtch_mk->buf, BUF_SZ, "Found new match (match size: %d)\n", match_sz);
        log_msg(STDOUT_FILENO, MATCHMAKER, mtch_mk->buf);
        announce_match_to_clients(m, mtch_mk->port_id++);
    }
}

void add_player(Match* m, int fd) {
    snprintf(m->buf, BUF_SZ, "trying to add fd: %d to match, match_sz: %d\n", fd, m->match_sz);
    log_msg(STDOUT_FILENO, MATCHMAKER, m->buf);
    for (int i = 0; i < m->match_sz; i++) {
        if (m->player_fds[i] == 0) {
            m->player_fds[i] = fd;
            break;
        }
    }

    snprintf(m->buf, BUF_SZ, "current matchplayers: [");
    write_msg(STDOUT_FILENO, m->buf);
    for (int i = 0; i < m->match_sz; i++) {
        snprintf(m->buf, BUF_SZ, "%d ", m->player_fds[i]);
        write_msg(STDOUT_FILENO, m->buf);
    }
    snprintf(m->buf, BUF_SZ, "]\n");
    write_msg(STDOUT_FILENO, m->buf);

    m->player_cnt += 1;
    snprintf(m->buf, BUF_SZ, "Added to match successfully (match players: %d)\n", m->player_cnt);
    log_msg(fd, MATCH, m->buf);
}

int check_for_ready_match(Match* m) {
    if (m->player_cnt == m->match_sz)
        return 1;
    return 0;
}

void announce_match_to_clients(Match* m, int port) {
    for (int i = 0; i < m->match_sz; i++) {
        snprintf(m->buf, BUF_SZ, "start_match port %d id %d\n", port, i);
        log_msg(m->player_fds[i], MATCH, m->buf);
    }

    memset(m->player_fds, 0, m->match_sz * sizeof(int));
    m->player_cnt = 0;
    snprintf(m->buf, BUF_SZ, "match announced to players (match size: %d)\n", m->match_sz);
    log_msg(STDOUT_FILENO, MATCH, m->buf);
}