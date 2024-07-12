#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include "etl/async.h"
#include "etl/mutex.h"
#include "etl/map.h"
#include "etl/time.h"
#include "periph/all.h"
#include "wizchip/ethernet.h"
#include <string>

extern "C" {
    extern char blinkSymbols[16];
    extern int blinkIsRunning;
    extern uint32_t blinkDelay;
    void panic(const char* msg);
}

namespace Project::periph {
    extern Encoder encoder4;
    extern I2S i2s2;
    extern PWM pwm2channel1;
    extern UART uart1;
    extern UART uart2;
}

namespace Project {
    extern etl::Tasks tasks;
    extern etl::Mutex mutex;
    extern etl::String<128> f;
    extern wizchip::Ethernet ethernet;
    
    class App {
        typedef void(*function_t)();
        static inline constexpr size_t cnt_max = 64;
        static function_t functions[cnt_max];
        static const char* names[cnt_max];
        static int cnt;

    public:
        App(const char* name, function_t test);
        static void run(const char* filter = "*");
    };

    struct Terminal {
        using HandlerFunction = std::function<etl::Result<const char*, const char*>(const etl::StringSplit<16>&)>;
        periph::UART& uart;
        etl::Map<const char*, HandlerFunction> routers = {};
        
        void init();

        template <typename F>
        auto add(const char* name, F&& handler);

        template <typename F>
        auto add_async(const char* name, F&& handler);

    private:
        void process(const uint8_t* buf, size_t len);

        void response(etl::Result<const char*, const char*> result);

        template <typename R, typename... Args>
        auto route(const char* name, std::function<R(Args...)> handler);

        template <typename R, typename... Args>
        auto route_async(const char* name, std::function<R(Args...)> handler);

        template <typename T>
        static T convert_arg(const etl::StringView& sv);

        template <typename Tuple, size_t... I>
        static Tuple make_tuple_from_args(const etl::StringSplit<16>& sv_args, std::index_sequence<I...>);
        
        template <typename... Args>
        static auto _tuple_convert_string_view_to_std_string(const std::tuple<Args...>& tpl);
        
        template <typename... Args>
        static auto _tuple_convert_std_string_to_string_view(const std::tuple<Args...>& tpl);
    };

    extern Terminal terminal;
}

#define APP(name) \
    static void unit_app_function_##name(); \
    static ::Project::App unit_app_##name(#name, unit_app_function_##name); \
    static void unit_app_function_##name()

template <typename F>
auto Project::Terminal::add(const char* name, F&& handler) {
    return route(name, std::function(std::forward<F>(handler)));
}

template <typename F>
auto Project::Terminal::add_async(const char* name, F&& handler) {
    return route_async(name, std::function(std::forward<F>(handler)));
}

template <typename R, typename... Args>
auto Project::Terminal::route(const char* name, std::function<R(Args...)> handler) {
    static_assert(etl::is_same_v<R, void> || etl::is_same_v<R, const char*> || etl::is_same_v<R, etl::Result<const char*, const char*>>);

    routers[name] = [handler](const etl::StringSplit<16>& sv_args) -> etl::Result<const char*, const char*> {
        if (sv_args.len() != sizeof...(Args) + 1) {
            return etl::Err("Argument size does not match");
        }

        std::tuple<Args...> arg_values = make_tuple_from_args<std::tuple<Args...>>(sv_args, std::index_sequence_for<Args...>{});
        if constexpr (etl::is_same_v<R, void>) {
            std::apply(handler, arg_values);
            return etl::Ok("");
        } else if constexpr (etl::is_same_v<R, const char*>) {
            return etl::Ok(std::apply(handler, arg_values));
        } else {
            return std::apply(handler, arg_values);
        }
    };

    return handler;
}

template <typename R, typename... Args>
auto Project::Terminal::route_async(const char* name, std::function<R(Args...)> handler) {
    static_assert(etl::is_same_v<R, void> || etl::is_same_v<R, const char*> || etl::is_same_v<R, etl::Result<const char*, const char*>>);

    routers[name] = [this, handler](const etl::StringSplit<16>& sv_args) -> etl::Result<const char*, const char*> {
        if (sv_args.len() != sizeof...(Args) + 1) {
            return etl::Err("Argument size does not match");
        } else if (tasks.resources() == 0) {
            return etl::Err("No available task");
        }

        etl::async([](Terminal* self, const std::function<R(Args...)>* handler, std::tuple<Args...> arg_values) {
            auto arg_buffer = _tuple_convert_string_view_to_std_string(arg_values);
            auto values = _tuple_convert_std_string_to_string_view(arg_buffer);

            if constexpr (etl::is_same_v<R, void>) {
                std::apply(*handler, values);
                self->response(etl::Ok(""));
            } else if constexpr (etl::is_same_v<R, const char*>) {
                const char* res = std::apply(*handler, values);
                self->response(etl::Ok(res));
            } else {
                etl::Result<const char*, const char*> res = std::apply(*handler, values);
                self->response(res);
            }
        }, this, &handler, make_tuple_from_args<std::tuple<Args...>>(sv_args, std::index_sequence_for<Args...>{}));
        
        return etl::Err("");
    };

    return handler;
}

template <typename T>
T Project::Terminal::convert_arg(const etl::StringView& sv) {
    if constexpr (etl::is_same_v<T, int>) {
        return sv.to_int();
    } else if constexpr (etl::is_same_v<T, float>) {
        return sv.to_float();
    } else if constexpr (etl::is_same_v<T, etl::StringView>) {
        return sv;
    } else if constexpr (etl::is_same_v<T, etl::Time>) {
        return etl::time::milliseconds(sv.to_int());
    }
}

template <typename Tuple, size_t... I>
Tuple Project::Terminal::make_tuple_from_args(const etl::StringSplit<16>& sv_args, std::index_sequence<I...>) {
    return std::make_tuple(convert_arg<std::tuple_element_t<I, Tuple>>(sv_args[I + 1])...);
}

template <typename... Args>
auto Project::Terminal::_tuple_convert_string_view_to_std_string(const std::tuple<Args...>& tpl) {
    return std::apply([](const Args&... args) {
        auto converter = [](const auto& value) {
            if constexpr (etl::is_same_v<etl::decay_t<decltype(value)>, etl::StringView>) {
                return std::string(value.data(), value.len());
            } else {
                return value;
            }
        };
        return std::make_tuple(converter(args)...);
    }, tpl);
}

template <typename... Args>
auto Project::Terminal::_tuple_convert_std_string_to_string_view(const std::tuple<Args...>& tpl) {
    return std::apply([](const Args&... args) {
        auto converter = [](const auto& value) {
            if constexpr (etl::is_same_v<etl::decay_t<decltype(value)>, std::string>) {
                return etl::string_view(value.data(), value.size());
            } else {
                return value;
            }
        };
        return std::make_tuple(converter(args)...);
    }, tpl);
}

#endif // PROJECT_PROJECT_H
