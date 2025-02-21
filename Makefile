CC = gcc
CFLAGS = -Wall -g 
LIBS = -lrelic -lcurl -lcjson -lssl -lcrypto -lm -l:libb64.a -luuid
LLLIB = -L/usr/local/lib
SOURCES = client.c utils/crypto.c utils/request.c utils/testing.c
HEADERS = utils/crypto.h utils/request.h utils/testing.h

all: client

client: $(SOURCES) 
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(LLLIB) 

.PHONY: clean

clean:
	rm -f client

