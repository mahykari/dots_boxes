#include "player.h"

volatile sig_atomic_t time_is_up = 0;

void handle_alarm(int s) { time_is_up = 1; }

Player* new_player(int plyr_id, int plyr_cnt) {
    Player* p = (Player*)malloc(sizeof(Player));
    p->plyr_id = plyr_id;
    p->str_buf = (String_Buf*)malloc(sizeof(String_Buf));
    p->g = new_game(plyr_cnt);
    return p;
}

void clear_player(Player* p) {
    strcpy(p->str_buf->msg_buf, "Clearing player...\n");
    log_msg(STDOUT_FILENO, SERVER, p->str_buf->msg_buf);
    close(p->sockfd);
    clear_game(p->g);
    
    free(p->str_buf);
    free(p);
}

void bind_player(Player* p, int port) {
    p->plyr_addr.sin_family = AF_INET;
    p->plyr_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    p->plyr_addr.sin_port = htons(port);
    if (bind(p->sockfd, (struct sockaddr*)&p->plyr_addr, sizeof(p->plyr_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void init_player(Player* p, int port) {
    p->sockfd = make_socket(SOCK_DGRAM);
    change_socket_options(p->sockfd, SO_BROADCAST);
    change_socket_options(p->sockfd, SO_REUSEADDR);
    bind_player(p, port);
    announce_init(PLAYER, p->str_buf, p->sockfd, p->plyr_addr);
}

void init_player_fd_set(Player* p, fd_set* readfds) {
    FD_ZERO(readfds);
    FD_SET(STDIN_FILENO, readfds);
    FD_SET(p->sockfd, readfds);
}

int find_player_input_src(Player* p, fd_set* readfds) {
    if (FD_ISSET(STDIN_FILENO, readfds))
        return STDIN_FILENO;
    if (FD_ISSET(p->sockfd, readfds))
        return p->sockfd;

    return -1;
}

int handle_wall(Player* p) {
    char tok[MAX_ARGS][BUF_SZ];
    memcpy(tok, p->str_buf->token_buf, MAX_ARGS * BUF_SZ);
    Point p1 = {atoi(tok[P1_ROW]), atoi(tok[P1_COL])};
    Point p2 = {atoi(tok[P2_ROW]), atoi(tok[P2_COL])};
    Point_Pair p_pair = {p1, p2};
    process_move(p->g, &p_pair);
    if (game_is_over(p->g))
        return 1;
    
    return 0;
}

int process_player_input(Player* p, int fd) {
    struct sockaddr_in temp = p->plyr_addr;
    socklen_t len = sizeof(temp);
    flush_bufs(p->str_buf);
    if (fd == STDIN_FILENO) {
        int read_res = read(fd, p->str_buf->read_buf, BUF_SZ);
        if (read_res == 0)
            return 1;
        sendto(p->sockfd, p->str_buf->read_buf, BUF_SZ, 0,
                (struct sockaddr*)&temp, len);
        tokenize(p->str_buf, p->str_buf->read_buf);
        return handle_wall(p);
    }
    
    else {
        recvfrom(fd, p->str_buf->read_buf, BUF_SZ, 0,
                (struct sockaddr*)&temp, &len);
        write_msg(STDOUT_FILENO, p->str_buf->read_buf);
        tokenize(p->str_buf, p->str_buf->read_buf);
        if (atoi(p->str_buf->token_buf[ID]) != p->plyr_id) {
            return handle_wall(p);
        }
    }

    return 0;
}

int run_player(Player* p) {
    fd_set* readfds = (fd_set*)malloc(sizeof(fd_set));
    announce_player(p);
    announce_run(PLAYER);
    alarm(PLYR_TIME);
    for (;;) {
        signal(SIGALRM, handle_alarm);
        init_player_fd_set(p, readfds);
        int res = select(p->sockfd + 1, readfds, NULL, NULL, NULL);
        if (res == -1) {
            if (errno == EINTR) {
                time_is_up = 0;
                change_current_player(p->g);
                alarm(PLYR_TIME);
                continue;
            }

            else {
                perror("select");
                exit(EXIT_FAILURE);
            }
        }
        announce_event(PLAYER);

        
        int fd = find_player_input_src(p, readfds);
        snprintf(p->str_buf->msg_buf, BUF_SZ, "Recieved packet (fd: %d)\n", fd);
        log_msg(STDOUT_FILENO, PLAYER, p->str_buf->msg_buf);
        
        alarm(PLYR_TIME);
        int game_res = process_player_input(p, fd);
        if (game_res)
            break;
    }

    announce_result(p->g);
    free(readfds);
    return 0;
}

void announce_player(Player* p) {
    snprintf(p->str_buf->msg_buf, BUF_SZ, "player id: %d\n", p->plyr_id);
    log_msg(STDOUT_FILENO, PLAYER, p->str_buf->msg_buf);
}