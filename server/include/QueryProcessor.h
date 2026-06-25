#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class StoredDataManager;

struct QueryContext {
    std::string database;
};

class QueryProcessor {
public:
    explicit QueryProcessor(StoredDataManager& storage);
    nlohmann::json executeScript(const std::string& script, QueryContext& context);

private:
    StoredDataManager& storage_;
    nlohmann::json executeStatement(const std::string& statement, QueryContext& context);
};
