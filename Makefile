CC = gcc
CFLAGS = -Wall -g 
LIBS = -lrelic -lcurl -lcjson -l:libb64.a -luuid -pthread
SOURCES = client.c utils/crypto.c utils/request.c utils/testing.c utils/memory.c
HEADERS = utils/crypto.h utils/request.h utils/testing.h utils/memory.h

all: normal

normal: $(SOURCES)
	$(CC) $(SOURCES) -o client $(CFLAGS) $(LIBS)

test: $(SOURCES)
	$(CC) $(SOURCES) -o test_client $(CFLAGS) -DTEST_MODE=1 $(LIBS)

clean:
	rm -f client test_client 
