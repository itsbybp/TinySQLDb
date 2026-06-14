#include "ApiServer.h"
#include "QueryProcessor.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;
using namespace tinydb;

ApiServer::ApiServer(int port) : port_(port) {}

void ApiServer::setQueryProcessor(std::shared_ptr<QueryProcessor> qp) {
    qp_ = std::move(qp);
}

void ApiServer::run() {
    httplib::Server svr;

    svr.Options("/query", [&](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200;
    });

    svr.Post("/query", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        try {
            auto j = json::parse(req.body);
            std::string sql = j.value("sql", "");
            std::string database;
            if (j.contains("context") && j["context"].is_object()) {
                database = j["context"].value("database", "");
            }

            if (!qp_) {
                json out = { {"status","error"}, {"message","QueryProcessor not set"} };
                res.set_content(out.dump(), "application/json");
                res.status = 500;
                return;
            }

            auto result = qp_->execute(sql, database);
            res.set_content(result.to_json().dump(), "application/json");
            res.status = 200;
        } catch (const std::exception& ex) {
            json out = { {"status","error"}, {"message", std::string("Invalid JSON or server error: ") + ex.what()} };
            res.set_content(out.dump(), "application/json");
            res.status = 400;
        }
    });

    std::cout << "API server listening on port " << port_ << "\n";
    svr.listen("0.0.0.0", port_);
}