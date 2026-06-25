#include "ApiServer.h"
#include "ApiHandler.h"
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

ApiServer::ApiServer(std::string host, int port) : host_(std::move(host)), port_(port) {}

void ApiServer::run() {
    httplib::Server server;
    ApiHandler handler;

    server.Options("/query", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });

    server.Post("/query", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        try {
            auto body = nlohmann::json::parse(req.body);
            auto response = handler.handleQuery(body);
            res.status = response.value("ok", false) ? 200 : 400;
            res.set_content(response.dump(2), "application/json");
        } catch (const std::exception& ex) {
            nlohmann::json error = {{"ok", false}, {"error", ex.what()}};
            res.status = 400;
            res.set_content(error.dump(2), "application/json");
        }
    });

    server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"ok\":true}", "application/json");
    });

    std::cout << "TinySQLDb server listening on http://" << host_ << ':' << port_ << std::endl;
    server.listen(host_, port_);
}
