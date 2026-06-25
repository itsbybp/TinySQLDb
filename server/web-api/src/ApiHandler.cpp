#include "ApiHandler.h"
#include "QueryProcessor.h"
#include "StoredDataManager.h"
#include <exception>
#include <string>

nlohmann::json ApiHandler::handleQuery(const nlohmann::json& requestBody) {
    static StoredDataManager storage("data");
    static QueryProcessor processor(storage);
    QueryContext context;

    try {
        if (requestBody.contains("database") && requestBody["database"].is_string()) {
            context.database = requestBody["database"].get<std::string>();
        }
        std::string script = requestBody.value("script", requestBody.value("query", ""));
        if (script.empty()) throw std::runtime_error("Request must include a non-empty script or query field");
        return processor.executeScript(script, context);
    } catch (const std::exception& ex) {
        return nlohmann::json{{"ok", false}, {"error", ex.what()}};
    }
}
