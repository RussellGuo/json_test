#include <arpa/inet.h>
#include <assert.h>
#include <bits/stdc++.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <memory>

constexpr auto UDP_TIM_PORT = 10000;

enum idx_of_timespec_array_t {
    REQ_SENDING,
    REQ_RECVING,
    RES_SENDING,
    RES_RECVING,
    TOTAL,
};

struct udp_time_package_t {
    timespec time_array[idx_of_timespec_array_t::TOTAL];
    size_t seq;
};

static inline void fill_time(timespec *ts) {
    int ret = clock_gettime(CLOCK_MONOTONIC, ts);
    (void)ret;
    assert(ret == 0);
}

static inline void check_syscall_result(bool is_ok, const char *reason = "unknown") {
    if (!is_ok) {
        throw std::runtime_error(std::string(reason) + ":" + strerror(errno));
    }
}

[[noreturn]] static void udp_time_server() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_syscall_result(sockfd >= 0, "create socket");

    struct sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;  // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_TIM_PORT);

    int ret;
    ret = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    check_syscall_result(ret >= 0, "bind");

    while (1) {
        struct {
            udp_time_package_t time_package;
            uint8_t dummy;
        } data;

        socklen_t len = sizeof(cliaddr);
        ssize_t n = recvfrom(sockfd, (char *)&data, sizeof(data), MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        fill_time(&data.time_package.time_array[REQ_RECVING]);
        check_syscall_result(n >= 0, "recvfrom");

        if (n == 0) {
            continue;
        }
        if (n != sizeof(udp_time_package_t)) {
            std::fprintf(stderr, "recv wrong request package with len = %zd\n", n);
            continue;
        }
        fill_time(&data.time_package.time_array[RES_SENDING]);

        n = sendto(sockfd, (const char *)&data.time_package, sizeof(data.time_package), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        check_syscall_result(n >= 0, "sendto");
    }
}

static void udp_time_client(const char *server_addr_str) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check_syscall_result(sockfd >= 0, "create socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(UDP_TIM_PORT);
    servaddr.sin_addr.s_addr = inet_addr(server_addr_str);
    ;

    if (servaddr.sin_addr.s_addr == INADDR_NONE) {
        throw std::runtime_error(std::string(server_addr_str) + " to addr failed");
    }

    size_t seq = 0;

    for (;;) {
        struct {
            udp_time_package_t time_package;
            uint8_t dummy;
        } time_package_response;
        memset(&time_package_response, 0, sizeof time_package_response);

        udp_time_package_t time_package_request;
        memset(&time_package_request, 0, sizeof time_package_request);
        time_package_request.seq = ++seq;
        fill_time(&time_package_request.time_array[REQ_SENDING]);
        ssize_t n = sendto(sockfd, (char *)&time_package_request, sizeof time_package_request, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        check_syscall_result(n >= 0, "sendto in client sending");
        if (n != sizeof time_package_request) {
            throw std::runtime_error(std::string("sendto in client sending return ") + std::to_string(n));
        }

        socklen_t len = sizeof servaddr;
        n = recvfrom(sockfd, (char *)&time_package_response, sizeof time_package_response, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
        fill_time(time_package_response.time_package.time_array + RES_RECVING);
        check_syscall_result(n >= 0, "recvfrom in client recving");
        if (n != sizeof time_package_response.time_package) {
            throw std::runtime_error(std::string("recvfrom in client recving return ") + std::to_string(n));
        }

        std::cout << time_package_response.time_package.seq;
        for (const auto &t : time_package_response.time_package.time_array) {
            auto ns = t.tv_sec * 1000ULL * 1000ULL * 1000ULL + t.tv_nsec;
            std::cout << "," << ns;
        }
        std::cout << std::endl;

        usleep(300ul * 1000);
    }

    close(sockfd);
}

[[noreturn]] static void usage(char *argv[]) {
    fprintf(stderr, "Usage:\n    %s -d\n    %s server-IP\n", argv[0], argv[0]);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv);
    }
    const char *const opt_argv = argv[1];

    if (strcmp(opt_argv, "-d") == 0) {
        // daemon mode
        udp_time_server();
    } else {
        const char *const server_addr_str = opt_argv;
        for (int i = 0; i < 10; i++) {
            udp_time_client(server_addr_str);
            sleep(5);
        }
    }
    return 0;
}
