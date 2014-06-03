#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <hiredis/hiredis.h>

#include "util.h"

#define STRATEGY_NONE 0
#define STRATEGY_FLUSH 1
#define STRATEGY_REPLACE 2
#define STRATEGY_IGNORE 3

typedef struct {
    opts_base_t base;
    short int strategy;
} opts_restore_t;

void parse_argv_for_restore(int argc, char * argv[], opts_restore_t * opts)
{
    parse_argv(argc, argv, &(opts->base));

    int c;
    opterr = 0;
    optind = 1;

    while ((c = getopt(argc, argv, "fri")) != -1) {
        switch (c) {
            case 'f':
                opts->strategy = STRATEGY_FLUSH;
                return;

            case 'r':
                opts->strategy = STRATEGY_REPLACE;
                return;

            case 'i':
                opts->strategy = STRATEGY_IGNORE;
                return;

            default:
                break;
        }
    }
}

int main(int argc, char * argv[])
{
    opts_restore_t opts = {{DEFAULT_HOST, DEFAULT_PORT, NULL, DEFAULT_DBNUM}, STRATEGY_NONE};

    parse_argv_for_restore(argc, argv, &opts);

    redisContext * conn = connect_to_redis(&(opts.base));

    redisReply * rply;
    int unsigned i = 0, j = 0;
    char * key = (char*) malloc(KEY_SIZE + 1);
    char * dump_str = (char* ) malloc(BUFF_SIZE + 1);
    char * dump_bin = (char* ) malloc(BUFF_SIZE + 1);
    char * ttl = (char*) malloc(KEY_SIZE + 1);
    const char * rstr_argv[4] = {"RESTORE", key, ttl, dump_bin};
    size_t rstr_argvlen[4] = {7, 0, 0, 0};
    char * cmd = (char*) malloc(CMD_SIZE + 1);
    short int key_exists = 0;

    FILE * fin = fdopen(0, "r");
    if (fin == NULL) {
        fprintf(stderr, "error: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    if (opts.strategy == STRATEGY_FLUSH) {
        rply = process_redis_command(conn, "FLUSHDB");
        freeReplyObject(rply);
    };

    while (!feof(fin)) {
        ++i;

        j = fscanf(fin, "%s %s %s", key, ttl, dump_str);
        if (j != 3) {
            if (j != EOF) {
                fprintf(stderr, "#%d error: %s\n", i, strerror(errno));
            }
            continue;
        }

        if (opts.strategy == STRATEGY_IGNORE) {
            sprintf(cmd, "EXISTS %s", key);
            rply = process_redis_command(conn, cmd);
            key_exists = rply->integer;
            freeReplyObject(rply);

            if (key_exists) continue;
        }

        if (opts.strategy == STRATEGY_REPLACE) {
            sprintf(cmd, "DEL %s", key);
            rply = process_redis_command(conn, cmd);
            freeReplyObject(rply);
        }

        hex2bin(dump_str, dump_bin);

        rstr_argvlen[1] = strlen(key);
        rstr_argvlen[2] = strlen(ttl);
        rstr_argvlen[3] = strlen(dump_str) / 2;

        rply = redisCommandArgv(conn, 4, rstr_argv, rstr_argvlen);

        if (rply == NULL) {
            fprintf(stderr, "#%d connection error: %s\n", i, conn->errstr);
            exit(EXIT_FAILURE);
        }

        if (rply->type == REDIS_REPLY_ERROR) {
            fprintf(stderr, "#%d Redis error: %s\n", i, rply->str);
        }

        freeReplyObject(rply);
    }

    redisFree(conn);

    return EXIT_SUCCESS;
}
