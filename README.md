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
####redis-dump, redis-restore

+ -h - host (ip allowed), default is 127.0.0.1
+ -p - port, default is 6379
+ -s - unix socket path
+ -n - db number, default is 0

####redis-dump only
+ -f - keys filter (wildcard allowed)

####redis-restore only
+ -f - flush db before restore
+ -r - replace existent keys
+ -i - ignore existent keys
