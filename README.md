# HTTPS

A minimal HTTPS client in C with OpenSSL.

It performs DNS resolution, TCP connection setup, TLS handshake, and so far for
manual construction of `HTTP/1.1` requests (e.g. `POST`) for interacting with
https servers for APIs such as GraphQL, which it was originally intended for.

Currently limited to `HTTP/1.1`, does not support higher-level features such as
response parsing, redirects, or `HTTP/2`.

## Resources
 * Docs: https://docs.openssl.org/master/man7/ossl-guide-libssl-introduction/
