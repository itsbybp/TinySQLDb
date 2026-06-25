#pragma once
#include "BTreeIndex.h"
#include "BSTIndex.h"
#include "CatalogManager.h"
#include "TableFile.h"
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>
class StoredDataManager {
    struct RuntimeIndex { IndexMetadata meta; std::shared_ptr<BSTIndex> bst; std::shared_ptr<BTreeIndex> btree; };
    std::filesystem::path rootDirectory_; CatalogManager catalog_; std::unordered_map<std::string,RuntimeIndex> indexes_;
    std::filesystem::path databasePath(const std::string&)const,tablePath(const std::string&,const std::string&)const;TableFile tableFile(const std::string&,const std::string&)const;
    static std::string indexKey(const std::string&,const std::string&);static int compareTyped(const std::string&,const std::string&,const std::string&);static bool matchesWhere(const Row&,const WhereClause&,const TableSchema&);
    void rebuildIndex(const IndexMetadata&);void rebuildIndexesForTable(const std::string&,const std::string&);long indexFind(const RuntimeIndex&,const std::string&)const;void indexInsert(RuntimeIndex&,const std::string&,long);
    static void quickSort(std::vector<Row>&,int,int,const std::string&,bool,const TableSchema&);
public:
    explicit StoredDataManager(std::filesystem::path rootDirectory="data");bool databaseExists(const std::string&)const;void createDatabase(const std::string&);void createTable(const std::string&,const std::string&,const std::vector<ColumnDefinition>&);void dropTable(const std::string&,const std::string&);void insertRow(const std::string&,const std::string&,const std::vector<std::string>&); void insertRows(const std::string&,const std::string&,const std::vector<std::vector<std::string>>&);nlohmann::json selectRows(const std::string&,const ParsedSelect&);int deleteRows(const std::string&,const std::string&,const WhereClause&);int updateRows(const std::string&,const ParsedUpdate&);void createIndex(const std::string&,const std::string&,const std::string&,const std::string&,const std::string&);
};
