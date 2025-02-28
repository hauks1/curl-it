CC = gcc
CFLAGS = -Wall -g 
LIBS = -lrelic -lcurl -lcjson -lssl -lcrypto -lm -l:libb64.a -luuid -pthread
LLLIB = -L/usr/local/lib 
SOURCES = client.c utils/crypto.c utils/request.c utils/testing.c utils/memory.c
HEADERS = utils/crypto.h utils/request.h utils/testing.h utils/memory.h

all: client

client: $(SOURCES) 
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(LLLIB) 

.PHONY: clean

clean:
	rm -f client test

