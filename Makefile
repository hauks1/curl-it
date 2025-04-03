CC = gcc
CFLAGS = -Wall -g -I. 
LIBS = -lrelic -lcurl -lcjson -l:libb64.a -luuid -pthread

SOURCES = client.c \
          testing/testing.c \
          core/message/message.c \
          core/crypto/utils/utils.c \
          core/crypto/mklhs/mklhs.c \
          core/crypto/love/love.c \
          core/request/request.c \
          core/send/send.c

HEADERS = testing/testing.h \
          core/message/message.h \
          core/crypto/utils/utils.h \
          core/crypto/mklhs/mklhs.h \
          core/crypto/love/love.h \
          core/request/request.h \
          core/send/send.h

CLIENT = client
TEST_CLIENT = test_client

all: $(CLIENT)

$(CLIENT): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(CLIENT) $(LIBS)
 
test: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -DTEST_MODE=1 $(SOURCES) -o $(TEST_CLIENT) $(LIBS)

clean:
	rm -f $(CLIENT) $(TEST_CLIENT)

.PHONY: all test clean