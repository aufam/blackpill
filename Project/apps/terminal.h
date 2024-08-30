#pragma once
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
        static T serialize_arg(etl::StringView sv);

        template <typename Tuple, size_t... I>
        static Tuple make_tuple_from_args(const etl::StringSplit<16>& sv_args, std::index_sequence<I...>);
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

        if constexpr (std::is_void_v<R>) {
            std::apply(handler, make_tuple_from_args<std::tuple<Args...>>(sv_args, std::index_sequence_for<Args...>{}));
            return etl::Ok("");
        } else if constexpr (etl::is_etl_result_v<R>) {
            R res = std::apply(handler, make_tuple_from_args<std::tuple<Args...>>(sv_args, std::index_sequence_for<Args...>{}));
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
            R res = std::apply(handler, make_tuple_from_args<std::tuple<Args...>>(sv_args, std::index_sequence_for<Args...>{}));
            return etl::Ok(fmt::format("{}", res));
        }
    };

    if (is_async) routers_async[name] = std::move(router); 
    else routers[name] = std::move(router); 

    return handler;
}

template <typename T>
T Project::Terminal::serialize_arg(etl::StringView sv) {
    if constexpr (std::is_integral_v<T>) {
        return sv.to_int();
    } else if constexpr (std::is_floating_point_v<T>) {
        return sv.to_float();
    } else if constexpr (std::is_same_v<T, etl::StringView>) {
        return sv;
    } else if constexpr (std::is_same_v<T, std::string_view> || std::is_same_v<T, std::string>) {
        return T(sv.data(), sv.len());
    } else if constexpr (std::is_same_v<T, etl::Time>) {
        return etl::time::milliseconds(sv.to_int());
    }
}

template <typename Tuple, size_t... I>
Tuple Project::Terminal::make_tuple_from_args(const etl::StringSplit<16>& sv_args, std::index_sequence<I...>) {
    return std::make_tuple(serialize_arg<std::tuple_element_t<I, Tuple>>(sv_args[I + 1])...);
}
