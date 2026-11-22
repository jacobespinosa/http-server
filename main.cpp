#include "server.h"

int main() {
    Server myServer(8080);

    myServer.start();
    myServer.stop();
}