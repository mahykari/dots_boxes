#include "server.h"
#include <stdio.h>

Server* new_server() {
    Server* s = (Server*)malloc(sizeof(Server));
    s->str_buf = (String_Buf*)malloc(sizeof(String_Buf));
    s->mtch_mk = new_matchmaker();
    memset(s->clntfd_arr, 0, MAX_PLYRS * sizeof(int));
    
    return s;
}

void bind_server(Server* s, int port) {
    s->serv_addr.sin_family = AF_INET;
    s->serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    s->serv_addr.sin_port = htons(port);
    
    if (bind(s->sockfd,(struct sockaddr*)&s->serv_addr, sizeof(s->serv_addr)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void start_listening(Server* s) {
    if (listen(s->sockfd, MAX_PLYRS) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void init_server(Server* s, int port) {
    s->sockfd = make_socket(SOCK_STREAM);
    change_socket_options(s->sockfd, SO_REUSEADDR);
    bind_server(s, port);
    start_listening(s);

    announce_init(SERVER, s->str_buf, s->sockfd, s->serv_addr);
}

void clear_server(Server* s) {
    strcpy(s->str_buf->msg_buf, "Clearing server...\n");
    log_msg(STDOUT_FILENO, SERVER, s->str_buf->msg_buf);
    close(s->sockfd);

    clear_matchmaker(s->mtch_mk);
    free(s->str_buf);
    free(s);
}

void run_server(Server* s) {
    fd_set* readfds = (fd_set*)malloc(sizeof(fd_set));
    
    for (;;) {
        int maxfd = init_server_fd_set(s, readfds);
        int res = select(maxfd + 1, readfds, NULL, NULL, NULL);
        if (res < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        
        announce_event(SERVER);

        if (FD_ISSET(s->sockfd, readfds)) {
            strcpy(s->str_buf->msg_buf, "New connection request\n");
            log_msg(STDOUT_FILENO, SERVER, s->str_buf->msg_buf);
            handle_new_client(s);
        }
        
        else {
            int fd = find_server_input_src(s, readfds);
            snprintf(s->str_buf->msg_buf, BUF_SZ, "Recieved packet (fd: %d)\n", fd);
            log_msg(STDOUT_FILENO, SERVER, s->str_buf->msg_buf);
            process_server_input(s, fd);
        }
    }

    free(readfds);
}

int init_server_fd_set(Server* s, fd_set* readfds) {
    FD_ZERO(readfds);
    FD_SET(STDIN_FILENO, readfds);
    FD_SET(s->sockfd, readfds);
    int res = add_clients_to_set(s, readfds);
    res = s->sockfd > res ? s->sockfd : res;
    return res;
}

void add_new_client(Server* s, int clntfd) {
    int* clnt_arr = s->clntfd_arr;
    for (int i = 0; i < MAX_PLYRS; i++) {
        if (clnt_arr[i] <= 0) {
            clnt_arr[i] = clntfd;
            break;
        }
    }
}

int add_clients_to_set(Server* s, fd_set* readfds) {
    int maxfd = 0;
    int* clntfd_arr = s->clntfd_arr;
    for (int i = 0; i < MAX_PLYRS; i++) {
        if (clntfd_arr[i] > 0) {
            int fd = clntfd_arr[i];
            FD_SET(fd, readfds);
            maxfd = fd > maxfd ? fd : maxfd;
        }
    }
    return maxfd;
}

int find_server_input_src(Server* s, fd_set* readfds) {
    if (FD_ISSET(STDIN_FILENO, readfds))
        return STDIN_FILENO;
    
    for (int i = 0; i < MAX_PLYRS; i++) {
        if (FD_ISSET(s->clntfd_arr[i], readfds)) {
            return s->clntfd_arr[i];
        }
    }

    return -1;
}

void process_server_input(Server* s, int fd) {
    int read_res;
    flush_bufs(s->str_buf);
    if (fd == STDIN_FILENO) {
        read_res = read(fd, s->str_buf->read_buf, BUF_SZ);
        if (read_res == 0) {
            clear_server(s);
            exit(EXIT_SUCCESS);
        }
    }

    else {
        read_res = recv(fd, s->str_buf->read_buf, BUF_SZ, 0);
        if (read_res == 0)
            handle_disconnected_client(s, fd);

        else {
            tokenize(s->str_buf, s->str_buf->read_buf);
            process_server_command(s, fd);
        }
    }
}

void process_server_command(Server* s, int fd) {
    if (strcmp(s->str_buf->token_buf[0], SET_MATCH_SZ) == 0) {
        int match_sz = atoi(s->str_buf->token_buf[1]);

        snprintf(s->str_buf->msg_buf, BUF_SZ, "set_match_size request (fd: %d)\n", fd);
        log_msg(STDOUT_FILENO, CLIENT, s->str_buf->msg_buf);
        handle_match_size(s, fd, match_sz);
    }
}