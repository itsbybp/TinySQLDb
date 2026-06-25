cmake_minimum_required(VERSION 3.16)
project(TinySQLDbServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tinysqldb_server
    src/main.cpp
    src/ApiServer.cpp
    src/QueryProcessor.cpp
    web-api/src/ApiHandler.cpp
    query-processor/src/Parser.cpp
    query-processor/src/Validator.cpp
    stored-data/src/CatalogManager.cpp
    stored-data/src/StoredDataManager.cpp
    stored-data/src/TableFile.cpp
    stored-data/src/BSTIndex.cpp
    stored-data/src/BTreeIndex.cpp
)

target_include_directories(tinysqldb_server PRIVATE
    include
    web-api/include
    query-processor/include
    stored-data/include
)

if (WIN32)
    target_link_libraries(tinysqldb_server PRIVATE ws2_32)
endif()
