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

    redisReply * keys_rply, * dump_rply;
    char * cmd = (char*) malloc(CMD_SIZE);
    int unsigned i;
    char * dump_str;
    long long ttl;

    keys_rply = process_redis_command(conn, "KEYS *");

    for (i = 0; i < keys_rply->elements; ++i) {
        sprintf(cmd, "PTTL %s", keys_rply->element[i]->str);
        dump_rply = process_redis_command(conn, cmd);
        ttl = dump_rply->integer;
        if (ttl < 0) ttl = 0;
        freeReplyObject(dump_rply);

        sprintf(cmd, "DUMP %s", keys_rply->element[i]->str);
        dump_rply = process_redis_command(conn, cmd);

        dump_str = (char*) malloc(dump_rply->len * 2 + 1);
        bin2hex(dump_rply->str, dump_rply->len, dump_str);
        freeReplyObject(dump_rply);

        printf("%s %lld %s\n", keys_rply->element[i]->str, ttl, dump_str);
        free(dump_str);
    }

    freeReplyObject(keys_rply);

    redisFree(conn);

    return EXIT_SUCCESS;
}
