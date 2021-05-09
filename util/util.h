#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../define.h"
#include "../string_buf/string_buf.h"

void log_msg(int fd, char* src, char* msg);
void write_msg(int fd, char* msg);
void send_msg(int sockfd, char* msg);
void write_dashes(int fd, int cnt);
void write_char(int fd, char c, int cnt);
void announce_event(char* src);
void announce_init(char* src, String_Buf* str_buf, int sockfd, struct sockaddr_in addr);
void announce_run(char* src);

int make_socket(int type);
void change_socket_options(int sockfd, int option);

int min(int a, int b);
int max(int a, int b);

#endif