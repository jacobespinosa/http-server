#include "server.h"
#include <netdb.h>
#include <unistd.h>

Server::Server(uint16_t port) {
    port_ = port;
}

void Server::start() {
    const int QUEUE_SIZE = 5;

    const std::string msg = "Connection made!";
    const char* msg_ptr = msg.data();
    size_t byte_len = msg.size();

    int status;
    int server_fd;
    int client_fd;
    addrinfo hints{};
    addrinfo* results = nullptr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags  = AI_PASSIVE;

    // TODO: Add error handling
    status = getaddrinfo(nullptr, std::to_string(port_).c_str(), &hints, &results);

    server_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    bind(server_fd, results->ai_addr, results->ai_addrlen);

    freeaddrinfo(results);

    listen(server_fd, QUEUE_SIZE);

    client_fd = accept(server_fd, nullptr, nullptr);

    send(client_fd, msg_ptr, byte_len, 0);
}

void Server::stop() {
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
}
