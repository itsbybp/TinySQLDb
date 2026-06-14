#include "ApiServer.h"
#include "QueryProcessor.h"
#include <memory>
#include <iostream>

int main() {
    auto qp = std::make_shared<tinydb::QueryProcessor>();
    tinydb::ApiServer server(8080);
    server.setQueryProcessor(qp);
    std::cout << "Starting tinyserver (HTTP) on port 8080\n";
    server.run(); // blocks
    return 0;
}