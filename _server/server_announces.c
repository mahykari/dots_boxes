#include "server.h"
#include <stdio.h>

void announce_init_server(Server* s) {
    char buf[BUF_SZ];
    snprintf(buf, BUF_SZ, "Server initialized (");
    strcpy(s->str_buf->msg_buf, buf);
    
    snprintf(buf, BUF_SZ, "socket fd: %d", s->sockfd);
    strcat(s->str_buf->msg_buf, buf);
    
    snprintf(buf, BUF_SZ, ", ip: %s", inet_ntoa(s->serv_addr.sin_addr));
    strcat(s->str_buf->msg_buf, buf);

    snprintf(buf, BUF_SZ, ", port: %d)\n", ntohs(s->serv_addr.sin_port));
    strcat(s->str_buf->msg_buf, buf);
    
    log_msg(STDOUT_FILENO, SERVER, s->str_buf->msg_buf);
    log_msg(STDOUT_FILENO, SERVER, "Press CTRL+D to quit\n");
}

void announce_new_client(Server* s, int clntfd, struct sockaddr_in* clnt_addr) {
    char buf[BUF_SZ];
    snprintf(buf, BUF_SZ, "Client accepted (");
    strcpy(s->str_buf->msg_buf, buf);
    
    snprintf(buf, BUF_SZ, "client fd: %d", clntfd);
    strcat(s->str_buf->msg_buf, buf);
    
    snprintf(buf, BUF_SZ, ", ip: %s", inet_ntoa(clnt_addr->sin_addr));
    strcat(s->str_buf->msg_buf, buf);

    snprintf(buf, BUF_SZ, ", port: %d)\n", ntohs(clnt_addr->sin_port));
    strcat(s->str_buf->msg_buf, buf);
    
    log_msg(STDOUT_FILENO, SERVER, s->str_buf->msg_buf);
}