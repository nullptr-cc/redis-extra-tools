Redis extra tools
=================

### Install
```
make
```

### Dependencies
[hiredis](https://github.com/redis/hiredis)

### Usage example

```
redis-dump -h redis.server.net -n 2 > dump.rd
redis-restore -s /var/run/redis.sock -n 4 < dump.rd
```

### CLI arguments
+ -h - host (ip allowed), default is 127.0.0.1
+ -p - port, default is 6379
+ -s - unix socket path
+ -n - db number, default is 0

### TODO
**redis-dump**

+ filter for keys

**redis-restore**

+ strategy - flushdb before restore, replace or ignore dumplicate keys

**redis-migrate**

new tool for partial migrating
