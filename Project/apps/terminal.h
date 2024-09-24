#pragma once
#include "etl/result.h"
#include <fmt/format.h>
#include <delameta/debug.h>
#include <delameta/stream.h>
#include <etl/string_view.h>
#include <etl/time.h>
#include <functional>
#include <unordered_map>

namespace Project {
    
    struct Terminal {
        using Result = etl::Result<std::string, std::string>;
        using HandlerFunction = std::function<Result(const etl::StringSplit<16>&)>;

        template <typename F>
        auto route(const char* name, F&& handler);

        template <typename F>
        auto route_async(const char* name, F&& handler);

        void execute(delameta::Descriptor& fd, etl::StringView cmd);

        std::unordered_map<const char*, HandlerFunction> routers = {};
        std::unordered_map<const char*, HandlerFunction> routers_async = {};

    private:
        template <typename R, typename... Args>
        auto route_(bool is_async, const char* name, std::function<R(Args...)> handler);

        template <typename T>
        static etl::Result<T, const char*> deserialize_arg(etl::StringView sv);

        template <typename Tuple, size_t... I>
        static auto make_tuple_from_args(const etl::StringSplit<16>& sv_args, std::index_sequence<I...>);

        template <typename T>
        struct always_false : std::false_type {};
    };
}

template <typename F>
auto Project::Terminal::route(const char* name, F&& handler) {
    return route_(false, name, std::function(std::forward<F>(handler)));
}

template <typename F>
auto Project::Terminal::route_async(const char* name, F&& handler) {
    return route_(true, name, std::function(std::forward<F>(handler)));
}

template <typename R, typename... Args>
auto Project::Terminal::route_(bool is_async, const char* name, std::function<R(Args...)> handler) {
    auto router = [handler](const etl::StringSplit<16>& sv_args) -> Result {
        if (sv_args.len() != sizeof...(Args) + 1) {
            return etl::Err("Argument size does not match");
        }

        std::tuple<etl::Result<Args, const char*>...> args_result =
            make_tuple_from_args<std::tuple<Args...>>(sv_args, std::index_sequence_for<Args...>{});

        // check for err
        const char* err = nullptr;
        auto check_err = [&](auto& item) {
            if (err == nullptr && item.is_err()) {
                err = item.unwrap_err();
            }
        };
        std::apply([&](auto&... args) { ((check_err(args)), ...); }, args_result);
        if (err) return etl::Err(err);

        if constexpr (std::is_void_v<R>) {
            std::apply([&](auto&... args) { handler(std::move(args.unwrap())...); }, args_result);
            return etl::Ok("");
        } else if constexpr (etl::is_etl_result_v<R>) {
            R res = std::apply([&](auto&... args) { return handler(std::move(args.unwrap())...); }, args_result);
            if (res.is_ok()) {
                if constexpr (std::is_void_v<etl::result_value_t<R>>) {
                    return etl::Ok("");
                } else {
                    return etl::Ok(fmt::format("{}", res.unwrap()));
                }
            } else {
                return etl::Err(fmt::format("{}", res.unwrap_err()));
            }
        } else {
            R res = std::apply([&](auto&... args) { return handler(std::move(args.unwrap())...); }, args_result);
            return etl::Ok(fmt::format("{}", res));
        }
    };

    if (is_async) routers_async[name] = std::move(router); 
    else routers[name] = std::move(router); 

    return handler;
}

template <typename T> Project::etl::Result<T, const char*>
Project::Terminal::deserialize_arg(etl::StringView sv) {
    if constexpr (std::is_integral_v<T>) {
        return etl::Ok(sv.to_int());
    } else if constexpr (std::is_floating_point_v<T>) {
        return etl::Ok(sv.to_float());
    } else if constexpr (std::is_same_v<T, etl::StringView>) {
        return etl::Ok(sv);
    } else if constexpr (std::is_same_v<T, std::string_view> || std::is_same_v<T, std::string>) {
        return etl::Ok(T(sv.data(), sv.len()));
    } else {
        always_false<T>::value;
    }
}

template<> inline Project::etl::Result<Project::etl::Time, const char*>
Project::Terminal::deserialize_arg(etl::StringView sv) {
    if (sv.find("ms") < sv.len()) {
        return etl::Ok(etl::time::milliseconds(sv.to_int()));
    } else if (sv.find("min") < sv.len()) {
        return etl::Ok(etl::time::minutes(sv.to_int()));
    } else if (sv.find("s") < sv.len()) {
        return etl::Ok(etl::time::seconds(sv.to_int()));
    } else {
        return etl::Err("Invalid time format");
    }
}

template <typename Tuple, size_t... I> auto
Project::Terminal::make_tuple_from_args(const etl::StringSplit<16>& sv_args, std::index_sequence<I...>) {
    return std::make_tuple(deserialize_arg<std::tuple_element_t<I, Tuple>>(sv_args[I + 1])...);
}
