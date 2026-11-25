// server.h
#pragma once

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <cstdint>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

class Server {
public:
    Server(uint16_t port);

    void start();
    void stop();

private:
    int server_fd = -1;
    uint16_t port_;

    int  accept_or_throw();
};