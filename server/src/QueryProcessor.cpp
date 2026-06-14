#include "QueryProcessor.h"
#include <algorithm>
#include <chrono>

using namespace tinydb;

QueryResult QueryProcessor::execute(const std::string& sql, const std::string& database) {
    QueryResult res;
    auto t0 = std::chrono::high_resolution_clock::now();

    std::string s = sql;
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });

    if (s.rfind("SELECT", 0) == 0) {
        res.ok = true;
        res.columns = { "id", "name" };
        res.rows = { { "1", "Alice" }, { "2", "Bob" } };
        res.message = "select stub";
    } else if (s.rfind("CREATE DATABASE", 0) == 0) {
        res.ok = true;
        res.message = "create database stub";
    } else if (s.rfind("SET DATABASE", 0) == 0) {
        res.ok = true;
        res.message = "set database validated (stub): " + database;
    } else {
        res.ok = false;
        res.message = "statement not implemented in stub";
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    res.duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    return res;
}