#include "util.h"

void log_msg(int fd, char* src, char* msg) {
    char buf[BUF_SZ];
    strcpy(buf, src);
    strcat(buf, msg);
    if (fd == STDOUT_FILENO)
        write_msg(fd, buf);
    else
        send_msg(fd, buf);
}

void write_msg(int fd, char* msg) {
    write(fd, msg, strlen(msg));
}

void send_msg(int sockfd, char* msg) {
    send(sockfd, msg, strlen(msg), 0);
}

void write_char(int fd, char c, int cnt) {
    char buf[2] = {c, '\0'};
    for (int i = 0; i < cnt; i++) {
        write(fd, buf, strlen(buf));
    }
}

void write_dashes(int fd, int cnt) {
    write_char(fd, '-', cnt);
    write_char(fd, '\n', 1);
}

void announce_event(char* src) {
    write_msg(STDOUT_FILENO, "\n");
    write_dashes(STDOUT_FILENO, 32);
    write_msg(STDOUT_FILENO, src);
    write_msg(STDOUT_FILENO, EVENT);
    write_dashes(STDOUT_FILENO, 32);
}

void announce_init(char* src, String_Buf* str_buf, int sockfd, struct sockaddr_in addr) {
    char buf[BUF_SZ];
    snprintf(buf, BUF_SZ, "init success (");
    strcpy(str_buf->msg_buf, buf);
    
    snprintf(buf, BUF_SZ, "socket fd: %d", sockfd);
    strcat(str_buf->msg_buf, buf);
    
    snprintf(buf, BUF_SZ, ", ip: %s", inet_ntoa(addr.sin_addr));
    strcat(str_buf->msg_buf, buf);

    snprintf(buf, BUF_SZ, ", port: %d)\n", ntohs(addr.sin_port));
    strcat(str_buf->msg_buf, buf);
    
    log_msg(STDOUT_FILENO, src, str_buf->msg_buf);
}

void announce_run(char* src) {
    write_msg(STDOUT_FILENO, "\n");
    write_dashes(STDOUT_FILENO, 32);
    write_msg(STDOUT_FILENO, src);
    write_msg(STDOUT_FILENO, RUNNING);
    write_dashes(STDOUT_FILENO, 32);
}

int make_socket(int type) {
    int sockfd = socket(AF_INET, type, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void change_socket_options(int sockfd, int option) {
    int en = 1;
    if(setsockopt(sockfd, SOL_SOCKET, option, (char*)&en, sizeof(en)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

int min(int a, int b) { return a < b ? a : b; }

int max(int a, int b) { return a > b ? a : b; }