#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

void parse_argv(int argc, char * argv[], opts_base_t * opts)
{
    int c;
    opterr = 0;
    optind = 1;

    while ((c = getopt(argc, argv, "h:p:s:n:")) != -1) {
        switch (c) {
            case 'h':
                opts->host = (char*) malloc(strlen(optarg) + 1);
                strcpy(opts->host, optarg);
                break;

            case 'p':
                opts->port = atoi(optarg);
                break;

            case 's':
                opts->socket = (char*) malloc(strlen(optarg) + 1);
                strcpy(opts->socket, optarg);
                break;

            case 'n':
                opts->dbnum = atoi(optarg);
                break;

            default:
                break;
        }
    }
}

redisContext * connect_to_redis(opts_base_t * opts)
{
    redisContext * conn;

    if (opts->socket != NULL) {
        conn = redisConnectUnix(opts->socket);
    } else {
        conn = redisConnect(opts->host, opts->port);
    }

    if (conn != NULL && conn->err) {
        fprintf(stderr, "Connection error: %s\n", conn->errstr);
        exit(EXIT_FAILURE);
    }

    char * cmd = (char*) malloc(12);
    sprintf(cmd, "SELECT %d", opts->dbnum);
    redisReply * rply = process_redis_command(conn, cmd);

    freeReplyObject(rply);
    free(cmd);

    return conn;
}

redisReply * process_redis_command(redisContext * conn, char * cmd)
{
    redisReply * rply = redisCommand(conn, cmd);

    if (rply == NULL) {
        fprintf(stderr, "Connection error: %s\n", conn->errstr);
        exit(EXIT_FAILURE);
    }

    if (rply->type == REDIS_REPLY_ERROR) {
        fprintf(stderr, "Redis error: %s\n", rply->str);
        exit(EXIT_FAILURE);
    }

    return rply;
}

void bin2hex(char * in, int len, char * out)
{
    int unsigned i;

    char table[] = "0123456789ABCDEF";

    for (i = 0; i < len; ++i) {
        out[i*2+0] = table[in[i] >> 4 & 0x0F];
        out[i*2+1] = table[in[i] & 0x0F];
    }

    out[len*2] = 0;
}

void hex2bin(char * in, char * out)
{
    int unsigned i, len = strlen(in);

    for (i = 0; i < len; i += 2) {
        out[i / 2] = (hdig2bin(in[i]) << 4) + hdig2bin(in[i+1]);
    }
}

char hdig2bin(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else {
        return c - 'A' + 0x0A;
    }
}
