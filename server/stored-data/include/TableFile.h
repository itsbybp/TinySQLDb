#pragma once
#include "Parser.h"
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>
using Row = std::map<std::string,std::string>;
struct TableSchema { std::string tableName; std::vector<ColumnDefinition> columns; };
class TableFile {
    std::filesystem::path tableDirectory_;
    std::filesystem::path schemaPath() const; std::filesystem::path rowsPath() const;
    static std::string encodeRow(const TableSchema&,const Row&); static Row decodeRow(const TableSchema&,const std::string&);
    static void crypt(std::string& data);
public:
    explicit TableFile(std::filesystem::path);
    bool exists() const; void create(const TableSchema&); TableSchema readSchema() const;
    std::vector<Row> readRows() const; std::vector<std::pair<long,Row>> readRowsWithOffsets() const; Row readRowAt(long offset) const;
    void writeRows(const TableSchema&,const std::vector<Row>&); long appendRow(const TableSchema&,const Row&); std::vector<long> appendRows(const TableSchema&,const std::vector<Row>&);
    bool isEmpty() const; void removeFiles();
};
