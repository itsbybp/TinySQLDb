#pragma once
#include <memory>
#include <string>
#include "QueryProcessor.h"

namespace tinydb {

    class ApiServer {
    public:
        explicit ApiServer(int port = 8080);
        void setQueryProcessor(std::shared_ptr<QueryProcessor> qp);
        void run(); // blocking
    private:
        int port_;
        std::shared_ptr<QueryProcessor> qp_;
    };

} // namespace tinydb