#pragma once
#include <fmt/format.h>
#include <delameta/json.h>
#include <delameta/utils.h>

namespace Project {
    struct mac_t {
        std::array<uint8_t, 6> value;
    };
    struct ip_t {
        std::array<uint8_t, 4> value;
    };
}

template <> struct fmt::formatter<Project::mac_t> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.end(); }

    template <typename FormatContext>
    inline auto format(const Project::mac_t& m, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), 
            "{:x}:{:x}:{:x}:{:x}:{:x}:{:x}", 
            m.value[0], m.value[1], m.value[2], m.value[3], m.value[4], m.value[5]
        );
    }
};

template <> inline size_t 
Project::etl::json::size_max(const Project::mac_t& m) {
    return size_max(m.value);
}
template <> inline std::string 
Project::etl::json::serialize(const Project::mac_t& m) {
    return fmt::format("\"{}\"", m);
}
template <> inline Project::etl::Result<void, const char*>
Project::etl::json::deserialize(const etl::Json& j, Project::mac_t& m) {
    if (j.error_message()) return etl::Err(j.error_message().data());
    if (!j.is_string()) return etl::Err("JSON is not a string");
    auto sv = j.to_string();

    auto items = sv.split<6>(":");
    if (items.len() != 6) {
        return Err("Invalid format");
    }
    for (int i = 0; i < 6; ++i) {
        auto std_sv = std::string_view(items[i].data(), items[i].len());
        auto convert_result = delameta::string_hex_into<uint8_t>(std_sv);
        if (convert_result.is_err()) {
            return etl::Err(convert_result.unwrap_err());
        }
        m.value[i] = convert_result.unwrap();
    }
    return etl::Ok();
}

template <> struct fmt::formatter<Project::ip_t> {
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.end(); }

    template <typename FormatContext>
    inline auto format(const Project::ip_t& m, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), 
            "{}.{}.{}.{}", 
            m.value[0], m.value[1], m.value[2], m.value[3]
        );
    }
};

template <> inline size_t 
Project::etl::json::size_max(const Project::ip_t& m) {
    return size_max(m.value);
}
template <> inline std::string 
Project::etl::json::serialize(const Project::ip_t& m) {
    return fmt::format("\"{}\"", m);
}
template <> inline Project::etl::Result<void, const char*>
Project::etl::json::deserialize(const etl::Json& j, Project::ip_t& m) {
    if (j.error_message()) return etl::Err(j.error_message().data());
    if (!j.is_string()) return etl::Err("JSON is not a string");
    auto sv = j.to_string();

    auto items = sv.split<4>(".");
    if (items.len() != 4) {
        return Err("Invalid format");
    }
    for (int i = 0; i < 6; ++i) {
        auto std_sv = std::string_view(items[i].data(), items[i].len());
        auto convert_result = delameta::string_dec_into<uint8_t>(std_sv);
        if (convert_result.is_err()) {
            return etl::Err(convert_result.unwrap_err());
        }
        m.value[i] = convert_result.unwrap();
    }

    return etl::Ok();
}