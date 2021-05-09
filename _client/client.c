#include "client.h"

Client* new_client() {
    Client* c = (Client*)malloc(sizeof(Client));
    c->str_buf = (String_Buf*)malloc(sizeof(String_Buf));
    c->p = NULL;
    return c;
}

void connect_to_server(Client* c, int port) {
    c->serv_addr.sin_family = AF_INET;
    c->serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    c->serv_addr.sin_port = htons(port);
    
    if (connect(c->sockfd, (struct sockaddr*)&c->serv_addr, sizeof(c->serv_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

void init_client(Client* c, int port) {
    c->sockfd = make_socket(SOCK_STREAM);
    connect_to_server(c, port);
    announce_init(CLIENT, c->str_buf, c->sockfd, c->serv_addr);
}

void clear_client(Client* c) {
    strcpy(c->str_buf->msg_buf, "Clearing client...\n");
    log_msg(STDOUT_FILENO, CLIENT, c->str_buf->msg_buf);
    close(c->sockfd);

    free(c->str_buf);
    if (c->p != NULL)
        clear_player(c->p);
    free(c);
}

void init_fd_set(Client* c, fd_set* readfds) {
    FD_ZERO(readfds);
    FD_SET(STDIN_FILENO, readfds);
    FD_SET(c->sockfd, readfds);
}

void talk_to_server(Client* c) {
    fd_set* readfds = (fd_set*)malloc(sizeof(fd_set));
    announce_run(CLIENT);
    for (;;) {
        init_fd_set(c, readfds);
        int res = select(c->sockfd + 1, readfds, NULL, NULL, NULL);
        if (res < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        announce_event(CLIENT);

        int fd = find_input_src(c, readfds);
        snprintf(c->str_buf->msg_buf, BUF_SZ, "Recieved packet (fd: %d)\n", fd);
        log_msg(STDOUT_FILENO, CLIENT, c->str_buf->msg_buf);
        
        int p_res = process_input(c, fd);
        if (p_res) {
            log_msg(STDOUT_FILENO, CLIENT, "Match announced from server\n");
            break;
        }
    }

    free(readfds);
}

int find_input_src(Client* c, fd_set* readfds) {
    if (FD_ISSET(STDIN_FILENO, readfds))
        return STDIN_FILENO;
    if (FD_ISSET(c->sockfd, readfds))
        return c->sockfd;
    
    return -1;
}

int process_command(Client* c) {
    if (strcmp(c->str_buf->token_buf[0], SET_MATCH_SZ) == 0)
        c->p = new_player(-1, atoi(c->str_buf->token_buf[1]));
    return 0;
}

int process_msg(Client* c) {
    for (int i = 0; i < MAX_ARGS - 1; i++) {
        if (strcmp(c->str_buf->token_buf[i], "MATCH:") == 0 &&
            strcmp(c->str_buf->token_buf[i + 1], START_MATCH) == 0) {
            init_player(c->p, atoi(c->str_buf->token_buf[i + 3]));
            c->p->plyr_id = atoi(c->str_buf->token_buf[i + 5]);
            return 1;
        }
    }

    return 0;
}

int process_input(Client* c, int fd) {
    int read_res;
    flush_bufs(c->str_buf);
    if (fd == STDIN_FILENO) {
        read_res = read(fd, c->str_buf->read_buf, BUF_SZ);
        if (read_res == 0) {
            clear_client(c);
            exit(EXIT_SUCCESS);
        }
        send(c->sockfd, c->str_buf->read_buf, BUF_SZ, 0);
        tokenize(c->str_buf, c->str_buf->read_buf);
        process_command(c);
        return 0;
    }
    
    else {
        read_res = recv(fd, c->str_buf->read_buf, BUF_SZ, 0);
        if (read_res == 0) {
            log_msg(STDOUT_FILENO, CLIENT, CLNT_PANIC);
            clear_client(c);
            exit(EXIT_FAILURE);
        }

        write_msg(STDOUT_FILENO, c->str_buf->read_buf);
        tokenize(c->str_buf, c->str_buf->read_buf);
        return process_msg(c);
    }
}