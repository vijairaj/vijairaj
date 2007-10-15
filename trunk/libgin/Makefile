# $Id$

CC		= gcc
CFLAGS	= -fPIC -Wall
LDFLAGS	= -shared -Wl,-soname=$(ENGINE) -lc -lcrypto

ENGINE	= libgin.so
ENGINE_OBJS	= gin.o rsa.o

SERVER = server
SERVER_OBJS = server.o

all: $(ENGINE) $(SERVER)

$(ENGINE): $(ENGINE_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(SERVER): $(SERVER_OBJS)
	$(CC) -o $@ -lssl $^

clean:
	rm -rf $(ENGINE_OBJS) $(ENGINE) $(SERVER) $(SERVER_OBJS)

genkey:
	openssl genrsa > server.prv.pem
	openssl genrsa > dummy.prv.pem
	openssl req -new -key server.prv.pem > server.csr.pem
	openssl x509 -req -signkey server.prv.pem < server.csr.pem > server.crt.pem
	cat server.{prv,crt}.pem > server.pem
	rm server.{prv,csr,crt}.pem

$(ENGINE_OBJS): Makefile gin.c trace.h rsa.h rsa.c

