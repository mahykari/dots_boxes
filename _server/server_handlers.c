#include "server.h"

void handle_new_client(Server* s) {
    struct sockaddr_in clnt_addr;
    int addr_len = sizeof(clnt_addr);
    int clntfd = accept(s->sockfd, (struct sockaddr*)&clnt_addr, (socklen_t*)&addr_len);
    add_new_client(s, clntfd);

    announce_new_client(s, clntfd, &clnt_addr);
    
    strcpy(s->str_buf->msg_buf, "Connenction accepted\n");
    log_msg(clntfd, SERVER, s->str_buf->msg_buf);
}

void handle_disconnected_client(Server* s, int fd) {
    for (int i = 0; i < MAX_PLYRS; i++) {
        if (s->clntfd_arr[i] == fd) {
            s->clntfd_arr[i] = 0;
        }
    }

    close(fd);
    snprintf(s->str_buf->msg_buf, BUF_SZ, "Client disconnected (client fd: %d)\n", fd);
    log_msg(STDOUT_FILENO, SERVER, s->str_buf->msg_buf);
}

void handle_match_size(Server* s, int fd, int match_sz) {
    add_player_to_match(s->mtch_mk, fd, match_sz);
}