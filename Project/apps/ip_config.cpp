#include <apps/ip_config.h>
#include <fmt/ranges.h>
#include <apps/app.h>
#include <apps/terminal.h>
#include <delameta/http/http.h>

using namespace Project;
namespace http = delameta::http;
using etl::Ok;
using etl::Err;
using etl::Result;
using delameta::string_hex_into;
using delameta::string_num_into;

extern mac_t eeprom_read_mac();
extern void eeprom_write_mac(mac_t mac);

    constexpr auto asda = delameta::string_dec_into<uint8_t>("10").unwrap();

extern ip_t eeprom_read_ip();
extern void eeprom_write_ip(ip_t ip);

extern ip_t eeprom_read_sn();
extern void eeprom_write_sn(ip_t sn);

extern ip_t eeprom_read_gw();
extern void eeprom_write_gw(ip_t gw);

extern ip_t eeprom_read_dns();
extern void eeprom_write_dns(ip_t dns);

extern Terminal terminal;
extern http::Http http_server;

template <> Result<mac_t, const char*>
Terminal::deserialize_arg(etl::StringView sv) {
    return delameta::json::deserialize<mac_t>('\"' + std::string(sv.data(), sv.len()) + '\"' );
}

template <> Result<ip_t, const char*>
Terminal::deserialize_arg(etl::StringView sv) {
    return delameta::json::deserialize<ip_t>('\"' + std::string(sv.data(), sv.len()) + '\"' );
}

APP(ip_config) {
    terminal.route("get-mac", &eeprom_read_mac);
    http_server.Get("/mac", {}, &eeprom_read_mac);

    terminal.route("get-ip", &eeprom_read_ip);
    http_server.Get("/ip", {}, &eeprom_read_ip);

    terminal.route("get-sn", &eeprom_read_sn);
    http_server.Get("/sn", {}, &eeprom_read_sn);

    terminal.route("get-gw", &eeprom_read_gw);
    http_server.Get("/gw", {}, &eeprom_read_gw);

    terminal.route("get-dns", &eeprom_read_dns);
    http_server.Get("/dns", {}, &eeprom_read_dns);

    terminal.route("set-mac", &eeprom_write_mac);
    http_server.Put("/mac", std::tuple{http::arg::json}, &eeprom_write_mac);

    terminal.route("set-ip", &eeprom_write_ip);
    http_server.Put("/ip", std::tuple{http::arg::json}, &eeprom_write_ip);

    terminal.route("set-sn", &eeprom_write_sn);
    http_server.Put("/sn", std::tuple{http::arg::json}, &eeprom_write_sn);

    terminal.route("set-gw", &eeprom_write_gw);
    http_server.Put("/gw", std::tuple{http::arg::json}, &eeprom_write_gw);

    terminal.route("set-dns", &eeprom_write_dns);
    http_server.Put("/dns", std::tuple{http::arg::json}, &eeprom_write_dns);
}
