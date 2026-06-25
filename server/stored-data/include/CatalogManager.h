#pragma once
#include "Parser.h"
#include <filesystem>
#include <map>
#include <string>
#include <vector>
struct IndexMetadata{std::string database,table,name,column,type;};
class CatalogManager{
    std::filesystem::path catalogDirectory_;
    std::filesystem::path databasesPath()const,tablesPath()const,columnsPath()const,indexesPath()const;
    static std::vector<std::string> readLines(const std::filesystem::path&);static void appendLine(const std::filesystem::path&,const std::string&);
public:
    explicit CatalogManager(std::filesystem::path);void initialize();bool databaseExists(const std::string&)const;void registerDatabase(const std::string&);void unregisterTable(const std::string&,const std::string&);void registerTable(const std::string&,const std::string&,const std::vector<ColumnDefinition>&);void registerIndex(const IndexMetadata&);std::vector<IndexMetadata> indexesForTable(const std::string&,const std::string&)const;std::vector<IndexMetadata> allIndexes()const;bool indexedColumnExists(const std::string&,const std::string&,const std::string&)const;
    std::vector<std::map<std::string,std::string>> selectSystemTable(const std::string&)const;
};
