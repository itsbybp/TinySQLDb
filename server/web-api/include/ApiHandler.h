#pragma once

#include <nlohmann/json.hpp>

class ApiHandler {
public:
    nlohmann::json handleQuery(const nlohmann::json& requestBody);
};
