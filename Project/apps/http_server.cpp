#include "main.hpp"
#include "wizchip/http/server.h"
#include "etl/heap.h"

using namespace Project;
using namespace wizchip::http;

// define some json rules for some http classes
JSON_DEFINE(Project::wizchip::http::Server::Router, 
    JSON_ITEM("methods", methods), 
    JSON_ITEM("path", path)
)

JSON_DEFINE(Project::wizchip::http::Server::Error, 
    JSON_ITEM("err", what)
)

JSON_DEFINE(Project::wizchip::URL, 
    JSON_ITEM("host", host), 
    JSON_ITEM("path", path), 
    JSON_ITEM("queries", queries)
)

// example custom struct
struct Foo {
    int num;
    std::string text;
};

JSON_DEFINE(Foo, 
    JSON_ITEM("num", num), 
    JSON_ITEM("text", text)
)

APP(http_server) {
    static Server app;

    // example: set additional global headers
    app.global_headers["Server"] = [](const Request&, const Response&) { 
        return "stm32-wizchip/" WIZCHIP_VERSION; 
    };
    
    app.global_headers["Content-Length"] = [](const Request&, const Response& res) { 
        return res.body.size() ? std::to_string(res.body.size()) : "";
    };
    
    app.global_headers["X-Response-Time"] = [](const Request&, const Response&) { 
        return std::to_string((int)etl::time::elapsed(app.get_elapsed_time()).ms()) + "ms";
    };

    // example: set custom error handler
    app.error_handler = [](Server::Error err, const Request&, Response& res) {
        res.status = err.status;
        res.body = etl::json::serialize(err);
        res.headers["Content-Type"] = "application/json";
    };

    // example: print hello
    app.Get("/hello", {}, 
    []() -> const char* {
        return "Hello world from stm32-wizchip/" WIZCHIP_VERSION;
    });

    // example: - get request param (in this case the body as string_view), - possible error return value
    app.Post("/body", std::tuple{arg::body},
    [](std::string_view body) -> etl::Result<std::string_view, Server::Error> {
        if (body.empty()) {
            return etl::Err(Server::Error{StatusBadRequest, "Body is empty"});
        } else {
            return etl::Ok(body);
        }
    });

    static const char* const access_token = "1234";
    
    auto get_token = [](const Request& req) -> etl::Result<std::string_view, Server::Error> {
        std::string_view token = "";
        if (req.headers.has("Authentication")) {
            token = req.headers["Authentication"];
        } else if (req.headers.has("authentication")) {
            token = req.headers["authentication"];
        } else {
            return etl::Err(Server::Error{StatusUnauthorized, "No auth provided"});
        }
        if (token == std::string("Bearer ") + access_token) {
            return etl::Ok(token);
        } else {
            return etl::Err(Server::Error{StatusUnauthorized, "Token doesn't match"});
        }
    };

    // example: 
    // - adding dependency (in this case is authentication token), 
    // - arg with default value, it will try to find "add" key in the request headers and request queries. 
    //   If not found, use the default value
    // - json deserialize request body to Foo
    app.Post("/foo", std::tuple{arg::depends(get_token), arg::default_val("add", 20), arg::json},
    [](std::string_view token, int add, Foo foo) -> Foo {
        return {foo.num + add, foo.text + ": " + std::string(token)}; 
    });

    // example:
    // multiple methods handler
    app.route("/methods", {"GET", "POST"}, std::tuple{arg::method},
    [](std::string_view method) {
        if (method == "GET") {
            return "Example GET method";
        } else {
            return "Example POST method";
        }
    });

    // example: print FreeRTOS heap status as Map (it will be json serialized)
    app.Get("/heap", {},
    []() -> etl::Map<const char*, size_t> {
        return {
            {"freeSize", etl::heap::freeSize},
            {"totalSize", etl::heap::totalSize},
            {"minimumEverFreeSize", etl::heap::minimumEverFreeSize}
        };
    });

    // example: print all routes of this app as json list
    app.Get("/routes", {},
    []() -> etl::Ref<const etl::LinkedList<Server::Router>> {
        return etl::ref_const(app.routers);
    });

    // example: print all headers
    app.Get("/headers", std::tuple{arg::headers},
    [](etl::Ref<const etl::UnorderedMap<std::string, std::string>> headers) {
        return headers;
    });

    // example: print all queries
    app.Get("/queries", std::tuple{arg::queries},
    [](etl::Ref<const etl::UnorderedMap<std::string, std::string>> queries) {
        return queries;
    });

    // example: print url
    app.Get("/url", std::tuple{arg::url},
    [](etl::Ref<const wizchip::URL> url) {
        return url;
    });

    app.start({.port=5000, .number_of_socket=4});
}
