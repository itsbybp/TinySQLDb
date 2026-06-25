#pragma once

#include <string>
#include <vector>

struct ColumnDefinition {
    std::string name;
    std::string type;
    int varcharLength = 0;
    bool nullable = true;
};

struct WhereClause {
    bool enabled = false;
    std::string column;
    std::string op;
    std::string value;
};

struct ParsedSelect {
    std::vector<std::string> columns;
    std::string tableName;
    WhereClause where;
    std::string orderByColumn;
    bool orderDescending = false;
};

struct ParsedUpdate {
    std::string tableName;
    std::string column;
    std::string value;
    WhereClause where;
};

class Parser {
public:
    static std::vector<std::string> splitStatements(const std::string& script);
    static std::string trim(const std::string& text);
    static std::string upper(std::string text);
    static std::vector<std::string> splitCsv(const std::string& text);
    static std::string unquote(const std::string& text);

    static std::string parseNameAfter(const std::string& statement, const std::string& keyword);
    static std::vector<ColumnDefinition> parseCreateTableColumns(const std::string& statement);
    static std::string parseCreateTableName(const std::string& statement);
    static std::string parseDropTableName(const std::string& statement);
    static std::string parseInsertTableName(const std::string& statement);
    static std::vector<std::string> parseInsertValues(const std::string& statement);
    static ParsedSelect parseSelect(const std::string& statement);
    static ParsedUpdate parseUpdate(const std::string& statement);
    static WhereClause parseWhere(const std::string& text);

    static std::string parseIndexName(const std::string& statement);
    static std::string parseIndexTable(const std::string& statement);
    static std::string parseIndexColumn(const std::string& statement);
    static std::string parseIndexType(const std::string& statement);
};
