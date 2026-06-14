#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace tinydb {

    struct QueryResult {
        bool ok = true;
        std::string message;
        std::vector<std::string> columns;
        std::vector<std::vector<std::string>> rows;
        uint64_t duration_ms = 0;

        nlohmann::json to_json() const {
            nlohmann::json j;
            j["status"] = ok ? "ok" : "error";
            j["message"] = message;
            j["duration_ms"] = duration_ms;
            if (ok) {
                j["columns"] = columns;
                j["rows"] = rows;
            }
            return j;
        }
    };

    class QueryProcessor {
    public:
        QueryProcessor() = default;
        // Execute SQL in the given database context. Synchronous, returns QueryResult.
        QueryResult execute(const std::string& sql, const std::string& database);
    };

} // namespace tinydb