
SRC = ../source

CC = cc

CFLAGS = -D_GNU_SOURCE -DINTEGRATION_TEST -std=gnu99 -O3 -pthread -I$(SRC) -I../pcre

LDFLAGS = -lpthread 

cache: test_cache.c share.o agent_cache.o alloc.o rwlock.o shared.o 
	$(CC) $(CFLAGS) -o cache test_cache.c agent_cache.o share.o alloc.o rwlock.o shared.o $(LDFLAGS)

alloc: test_alloc.c alloc.o share.o shared.o
	$(CC) $(CFLAGS) -o alloc test_alloc.c share.o alloc.o shared.o $(LDFLAGS)

rwlock: test_rwlock.c rwlock.o
	$(CC) $(CFLAGS) -o rwlock test_rwlock.c rwlock.o $(LDFLAGS)

agent_cache.o: $(SRC)/agent_cache.h share.o alloc.o rwlock.o
	$(CC) -c $(CFLAGS) $(SRC)/agent_cache.c

alloc.o: $(SRC)/alloc.h $(SRC)/alloc.c share.o 
	$(CC) -c $(CFLAGS) $(SRC)/alloc.c

rwlock.o: $(SRC)/rwlock.h $(SRC)/rwlock.c
	$(CC) -c $(CFLAGS) $(SRC)/rwlock.c

share.o: $(SRC)/share.h $(SRC)/share.c
	$(CC) -c $(CFLAGS) $(SRC)/share.c

shared.o: $(SRC)/shared.c
	$(CC) -c $(CFLAGS) $(SRC)/shared.c

all: cache alloc rwlock

clean:
	-rm -rf *.dSYM *.o cache rwlock alloc

