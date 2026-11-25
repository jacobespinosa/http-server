#include "server.h"
#include <system_error>

Server::Server(uint16_t port) {
    port_ = port;
}

void Server::start() {
    const int BACKLOG = 5;

    const std::string msg = "Connection made!\n";

    int status;
    int client_fd;
    addrinfo hints{};
    addrinfo* results = nullptr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags  = AI_PASSIVE;

    status = getaddrinfo(nullptr, std::to_string(port_).c_str(), &hints, &results);
    if (status != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
        return;
    }

    server_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (server_fd == -1) {
        std::perror("socket() failed");
        freeaddrinfo(results);
        return;
    }

    status = bind(server_fd, results->ai_addr, results->ai_addrlen);
    if (status == -1) {
        std::perror("bind() failed");
        freeaddrinfo(results);
        stop();
        return;
    }

    freeaddrinfo(results);

    status = listen(server_fd, BACKLOG);
    if (status == -1) {
        std::perror("listen() failed");
        stop();
        return;
    }

    while (true) {
        try {
            client_fd = accept_or_throw();
        } catch (const std::system_error error) {
            std::perror(error.what());
            break;
        }

        // TODO: handle partial sends and SIGPIPE
        ssize_t bytes_sent = send(client_fd, msg.data(), msg.size(), 0);
        if (bytes_sent == -1) {
            std::perror("send() failed");
            close(client_fd);
            continue;
        }
        close(client_fd);
    }
}

void Server::stop() {
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
}

int Server::accept_or_throw() {
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd != -1) {
            return client_fd;
        }

        int err = errno;

        // non-blocking socket, no pending connections
        // Revist when introducting them
        /*
        if (err == EAGAIN || err == EWOULDBLOCK) {
            break;
        }
        */

        // Transient errors: retry accepting
        if (err == EINTR || err == ECONNABORTED) {
            continue;
        }

        // fatal error: throw back to caller
        throw std::system_error(err, std::generic_category(), "accept() failed");
    }
}
