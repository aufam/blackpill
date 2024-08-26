#pragma once
#ifndef APP_BUFFER_SIZE
#define APP_BUFFER_SIZE 64
#endif

extern "C" void panic(const char* msg);

namespace Project {
    class App {
        typedef void(*function_t)();
        static function_t functions[APP_BUFFER_SIZE];
        static const char* names[APP_BUFFER_SIZE];
        static int cnt;

    public:
        App(const char* name, function_t test);
        static void run(const char* filter = "*");
    };
}

#define APP(name) \
    static void unit_app_function_##name(); \
    static ::Project::App unit_app_##name(#name, unit_app_function_##name); \
    static void unit_app_function_##name()


#define APP_ASYNC(name) \
    static void unit_app_function_##name##_async(); \
    static void unit_app_function_##name() { ::Project::etl::async(&unit_app_function_##name##_async); } \
    static ::Project::App unit_app_##name(#name, unit_app_function_##name); \
    static void unit_app_function_##name##_async()
