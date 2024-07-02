#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include "etl/async.h"
#include "etl/mutex.h"
#include "periph/all.h"
#include "wizchip/ethernet.h"

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
    class App;
}

class Project::App {
    typedef void(*function_t)();
    static function_t functions[64];
    static const char* names[64];
    static int cnt;

public:
    App(const char* name, function_t test);
    static void run(const char* filter = "*");
};

#define APP(name) \
    static void unit_app_function_##name(); \
    static ::Project::App unit_app_##name(#name, unit_app_function_##name); \
    static void unit_app_function_##name()

#endif // PROJECT_PROJECT_H
