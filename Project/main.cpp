#include "main.hpp"

namespace Project {
    etl::Tasks tasks;
    etl::Mutex mutex;
    etl::String<128> f;

    wizchip::Ethernet ethernet({
        .hspi=hspi1,
        .cs={.port=GPIOA, .pin=GPIO_PIN_4},
        .rst={.port=GPIOA, .pin=GPIO_PIN_1},
        .netInfo={ 
            .mac={0x00, 0x08, 0xdc, 0xff, 0xee, 0xdd},
            .ip={10, 20, 30, 2},
            .sn={255, 255, 255, 0},
            .gw={10, 20, 30, 1},
            .dns={10, 20, 30, 1},
            .dhcp=NETINFO_STATIC,
        },
    });
}

namespace Project::periph {
    Encoder encoder4 { .htim=htim4 };
    I2S i2s2 { .hi2s=hi2s2 };
    PWM pwm2channel1 { .htim=htim2, .channel=TIM_CHANNEL_1 };
    UART uart1 { .huart=huart1 };
    UART uart2 { .huart=huart2 };
}

using namespace Project;

extern "C" void project_init() {
    tasks.init();
    mutex.init();
    ethernet.init();
    periph::uart1.init();
    periph::uart2.init();

    App::run("*");
}

extern "C" void panic(const char* msg) {
    auto delay = [] {
        for (int i = 0; i < 500'0000; ++i);
    };

    tasks.terminate();
    portDISABLE_INTERRUPTS();

    for (;;) {
        periph::uart1 << f("panic: %s\n", msg);
        delay();
    }
}

App::App(const char* name, App::function_t test) {
    functions[cnt] = test;
    names[cnt++] = name;
}

void App::run(const char* fil) {
    auto filter = etl::string_view(fil);
    if (filter.len() == 0)
        return;
    
    for (int i = 0; i < cnt; ++i) {
        auto test = functions[i];
        auto name = names[i];

        if (filter == name or (filter.back() == '*' and ::strncmp(name, filter.data(), filter.len() - 1) == 0))
            test();
    }
}

App::function_t App::functions[64] = {};
const char* App::names[64] = {};
int App::cnt = 0;