#include "string_buf.h"

void flush_bufs(String_Buf* sb) {
    memset(sb->msg_buf, 0, BUF_SZ);
    memset(sb->read_buf, 0, BUF_SZ);
    memset(sb->token_buf, 0, MAX_ARGS * BUF_SZ);
}

void tokenize(String_Buf* sb, char* in) {
    char dup[BUF_SZ];
    memcpy(dup, in, BUF_SZ);
    sb->cnt = 0;
    char *token = strtok(dup, " \r\n");
    while (token != NULL) {
        strcpy(sb->token_buf[sb->cnt++], token);
        token = strtok(NULL, " \r\n");
    }
}