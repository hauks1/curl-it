CC = gcc
CFLAGS = -Wall -g 
LIBS = -lrelic -lcurl -lcjson -lssl -lcrypto -lm -l:libb64.a 
LLLIB = -L/usr/local/lib
SOURCES = client.c

all: client

client: $(SOURCES) 
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS) $(LLLIB) 

.PHONY: clean

clean:
	rm -f client

