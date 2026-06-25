#pragma once

#include <string>

class ApiServer {
public:
    ApiServer(std::string host, int port);
    void run();

private:
    std::string host_;
    int port_;
};
