#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "https.h"

#define URL_MAX 256
#define PORT_MAX 6
static char URL[URL_MAX];
// static char PORT[PORT_MAX];

/*
struct addrinfo {
    int     ai_flags;       // Input flags
    int     ai_family;      // Protocol family for socket()
    int     ai_socktype;    // Socket type for socket()
    int     ai_protocol;    // Protocol for socket()
    size_t  ai_addrlen;     // Length of ai_addr
    struct sockaddr *ai_addr; // Socket address for socket()
    char    *ai_canonname;  // Canonical name of host
    struct addrinfo *ai_next; // Pointer to next structure in linked list
};
*/

sockfd dns_lookup(const char *addr, const char *port)
{
    int status;
    sockfd fd=-1;

    struct addrinfo *res, *p = NULL;
    struct addrinfo hints = {
        .ai_family   = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM
    };

    status = getaddrinfo(addr, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) {
            perror("socket");
            continue;
        }

        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) { // success
            break;
        }

        perror("connect");
        close(fd);
        fd = -1;
    }

    if (res) freeaddrinfo(res);
    return fd;
}

https_t *setup_https(const char* addr, const char* port)
{
    // int n=0;
    // while((URL[n]=addr[n])!='\0') n++;
    // n=0;
    // while((PORT[n]=port[n])!='\0') n++;

    https_t *h = calloc(1, sizeof *h);
    if (!h)         goto cleanup;

    h->fd = dns_lookup(addr, port);
    if (h->fd < 0)  goto cleanup;

    h->ctx = SSL_CTX_new(TLS_client_method());
    if (!h->ctx)    goto cleanup;

    h->ssl = SSL_new(h->ctx);
    if (!h->ssl)    goto cleanup;

    SSL_set_fd(h->ssl, h->fd);
    SSL_set_tlsext_host_name(h->ssl, addr);

    if (SSL_connect(h->ssl) != 1) {
        ERR_print_errors_fp(stderr);
        goto cleanup;
    }
    return h;

cleanup:
    if (h) {
        if (h->ssl) SSL_free(h->ssl);
        if (h->ctx) SSL_CTX_free(h->ctx);
        if (h->fd >= 0) close(h->fd);
        free(h);
    }
    return NULL;
}

int https_write(https_t *h, const char *req)
{
    int ret = SSL_write(h->ssl, req, strlen(req));
    if (ret < 0) {
        ERR_print_errors_fp(stderr);
    }
    return ret;
}

int https_read(https_t *h, char *buf, size_t buf_size)
{
    int ret = SSL_read(h->ssl, buf, buf_size-1);
    if (ret < 0) {
        ERR_print_errors_fp(stderr);
    }
    return ret;
}

/* Detroys contexts, frees ssl structs and https_t and closes the
 * file descriptor socket
 * */
void cleanup_https(https_t *h){
    SSL_shutdown(h->ssl);
    close(h->fd);
    SSL_free(h->ssl);
    SSL_CTX_free(h->ctx);
    free(h);
}
