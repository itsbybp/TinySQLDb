#pragma once

#include "Parser.h"
#include <string>
#include <vector>

struct TableSchema;

class Validator {
public:
    static void validateDatabaseName(const std::string& name);
    static void validateTableName(const std::string& name);
    static void validateColumnDefinitions(const std::vector<ColumnDefinition>& columns);
    static void validateInsertValues(const TableSchema& schema, const std::vector<std::string>& values);
    static void validateColumnExists(const TableSchema& schema, const std::string& column);
    static void validateValueForType(const std::string& type, const std::string& value);
};
