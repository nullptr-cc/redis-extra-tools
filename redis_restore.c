#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <hiredis/hiredis.h>

#include "util.h"

int main(int argc, char * argv[])
{
    opts_t opts = {DEFAULT_HOST, DEFAULT_PORT, NULL, DEFAULT_DBNUM};

    parse_argv(argc, argv, &opts);

    redisContext * conn = connect_to_redis(&opts);

    redisReply * rstr_rply;
    int unsigned i = 0, j = 0;
    char * key = (char*) malloc(KEY_SIZE + 1);
    char * dump_str = (char* ) malloc(BUFF_SIZE + 1);
    char * dump_bin = (char* ) malloc(BUFF_SIZE + 1);
    char * ttl = (char*) malloc(KEY_SIZE + 1);
    const char * rstr_argv[4] = {"RESTORE", key, ttl, dump_bin};
    size_t rstr_argvlen[4] = {7, 0, 0, 0};

    FILE * fin = fdopen(0, "r");
    if (fin == NULL) {
        fprintf(stderr, "error: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    while (!feof(fin)) {
        ++i;

        j = fscanf(fin, "%s %s %s", key, ttl, dump_str);
        if (j != 3) {
            if (j != EOF) {
                fprintf(stderr, "#%d error: %s\n", i, strerror(errno));
            }
            continue;
        }

        hex2bin(dump_str, dump_bin);

        rstr_argvlen[1] = strlen(key);
        rstr_argvlen[2] = strlen(ttl);
        rstr_argvlen[3] = strlen(dump_str) / 2;

        rstr_rply = redisCommandArgv(conn, 4, rstr_argv, rstr_argvlen);

        if (rstr_rply == NULL) {
            fprintf(stderr, "#%d connection error: %s\n", i, conn->errstr);
            exit(EXIT_FAILURE);
        }

        if (rstr_rply->type == REDIS_REPLY_ERROR) {
            fprintf(stderr, "#%d Redis error: %s\n", i, rstr_rply->str);
        }

        freeReplyObject(rstr_rply);
    }

    redisFree(conn);

    return EXIT_SUCCESS;
}
