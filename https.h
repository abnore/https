#ifndef HTTPS_H
#define HTTPS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef int sockfd;

typedef struct{
    sockfd fd;
    SSL_CTX *ctx;
    SSL *ssl;
} https_t;

/* Both does a dns lookup and sets up the socket on the given port
 * returns a socket set up for this dns and port
 * */
sockfd dns_lookup(const char *addr, const char *port);

/* Sets up a TLS connection over HTTP with the given socket and returns
 * a https_t struct with the SSL context, struct and socket
 * Docs: https://docs.openssl.org/master/man7/ossl-guide-libssl-introduction/
 * */
https_t *setup_https(const char* addr, const char* port);

int https_write(https_t *h, const char *req);
int https_read(https_t *h, char *buf, size_t buf_size);

/* Detroys contexts, frees ssl structs and https_t and closes the
 * file descriptor socket
 * */
void cleanup_https(https_t *h);

#endif // HTTPS_H
