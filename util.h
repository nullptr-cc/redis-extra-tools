#ifndef __REDIS_DUMP_RESTORE_UTIL_H
#define __REDIS_DUMP_RESTORE_UTIL_H

#include <hiredis/hiredis.h>

typedef struct {
    char * host;
    int unsigned port;
    char * socket;
    int unsigned dbnum;
} opts_base_t;

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 6379
#define DEFAULT_DBNUM 0

#define KEY_SIZE 1024
#define CMD_SIZE 2048
#define BUFF_SIZE 8192

void parse_argv(int argc, char * argv[], opts_base_t * opts);

redisContext * connect_to_redis(opts_base_t * opts);
redisReply * process_redis_command(redisContext * conn, char * cmd);

void bin2hex(char * in, int len, char * out);
void hex2bin(char * in, char * out);
char hdig2bin(char c);

#endif
