#include "main.hpp"
#include "wizchip/http/server.h"
#include "wizchip/http/client.h"
#include "etl/heap.h"

using namespace Project;
using namespace Project::etl::literals;
using namespace wizchip::http;
using Project::etl::mv;

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

    app.Get("/panic", std::tuple{arg::arg("msg")}, 
    [](std::string msg) { 
        panic(msg.c_str()); 
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

    app.route("/ethernet_buffer_max", {"GET"}, {},
    []() {
        return getSn_TxMAX(0);
    });

    app.route("/redirect", {"GET", "POST"}, std::tuple{arg::method, arg::headers, arg::body, arg::arg("path")}, 
    [](
        std::string method, 
        etl::Ref<const etl::UnorderedMap<std::string, std::string>> headers, 
        std::string body, 
        std::string path
    ) -> etl::Result<std::string, Server::Error> {
        return request(method, path, {.headers=*headers, .body=mv | body}).wait(1s)
            .then([](Response res) {
                return mv | res.body;
            })
            .except([](osStatus_t) {
                return Server::Error{StatusRequestTimeout, "timeout"};
            });
    });

    app.start({.port=5000, .number_of_socket=4});
}
