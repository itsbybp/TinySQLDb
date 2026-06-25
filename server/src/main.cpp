#include "ApiServer.h"
#include <cstdlib>

int main() {
    const char* portEnv = std::getenv("TINYSQLDB_PORT");
    int port = portEnv ? std::atoi(portEnv) : 8080;
    ApiServer server("0.0.0.0", port);
    server.run();
    return 0;
}
