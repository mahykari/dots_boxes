#ifndef __STRING_BUF_H__
#define __STRING_BUF_H__

#include <string.h>
#include <stdio.h>
#include "../define.h"

typedef struct string_buf String_Buf;

struct string_buf
{
    char msg_buf[BUF_SZ];
    char read_buf[BUF_SZ];
    char token_buf[MAX_ARGS][BUF_SZ];
    int cnt;
};

void flush_bufs(String_Buf* sb);
void tokenize(String_Buf* sb, char* in);
#endif