CC = gcc
CFLAGS = -Wall -x c -c
LINK = gcc
LFLAGS = -lhiredis

all: redis-dump redis-restore

redis-dump: util.o
	${CC} ${CFLAGS} redis_dump.c
	${LINK} redis_dump.o util.o -o redis-dump ${LFLAGS}

redis-restore: util.o
	${CC} ${CFLAGS} redis_restore.c
	${LINK} redis_restore.o util.o -o redis-restore ${LFLAGS}

util.o:
	${CC} ${CFLAGS} util.c

clean:
	rm -f *.o redis-dump redis-restore
