#include <stdio.h>
#include <string.h>
#include "https.h"

/* -- This program will not run, but it will compile -- */

#define URL  "test.url"
#define PORT  "443"

int main(void)
{
    https_t *handler = setup_https(URL, PORT);

    if (!handler) {
        fprintf(stderr, "[ERROR] failed to set up https connection\n");
        return EXIT_FAILURE;
    }

    char *req = "this will be a valid https/json/grapql (etc.) request";

    https_write(handler, req);

    char buf[4096];
    int n;
    while ((n = https_read(handler, buf, sizeof(buf)-1)) > 0) {
        fwrite(buf, 1, n, stdout);
    }
    printf("\n");

    cleanup_https(handler);

    return EXIT_SUCCESS;
}
