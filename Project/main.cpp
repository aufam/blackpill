#include <apps/app.h>
#include <etl/async.h>
#include <etl/string_view.h>
#include <main.h>
#include <task.h>

using namespace Project;

extern "C" {
    void delameta_stm32_hal_init();
    void delameta_stm32_hal_wizchip_init();
    void delameta_stm32_hal_wizchip_set_net_info(
        const uint8_t mac[6], 
        const uint8_t ip[4], 
        const uint8_t sn[4], 
        const uint8_t gw[4], 
        const uint8_t dns[4]
    );
}

static const uint8_t mac[] = {0x00, 0x08, 0xdc, 0xff, 0xee, 0xdd};
static const uint8_t ip[] = {10, 20, 30, 2};
static const uint8_t sn[] = {255, 255, 255, 0};
static const uint8_t gw[] = {10, 20, 30, 1};
static const uint8_t dns[] = {10, 20, 30, 1};

extern "C" void project_init() {
    etl::task::init();
    delameta_stm32_hal_init();
    delameta_stm32_hal_wizchip_set_net_info(mac, ip, sn, gw, dns);
    delameta_stm32_hal_wizchip_init();
    App::run();
}

extern UART_HandleTypeDef huart1;

extern "C" void panic(const char* msg) {
    etl::task::terminate(); // terminate all tasks
    taskDISABLE_INTERRUPTS();
	__disable_irq();

    for (;;) {
        HAL_UART_Transmit(&huart1, (const uint8_t*)msg, ::strlen(msg), HAL_MAX_DELAY);
        for (int i = 0; i < 100'000'000; ++i);
    }
}

App::App(const char* name, App::function_t fn) {
    if (name == etl::string_view("")) {
        panic("App name cannot be empty");
    }
    if (cnt == APP_BUFFER_SIZE) {
        panic("App buffer is full");
    }
    functions[cnt] = fn;
    names[cnt++] = name;
}

void App::run(const char* fil) {
    auto filter = etl::string_view(fil);
    if (filter.len() == 0){
        panic("App run filter token cannot be empty");
    }
    
    for (int i = 0; i < cnt; ++i) {
        auto test = functions[i];
        auto name = names[i];

        if (filter == name or (filter.back() == '*' and ::strncmp(name, filter.data(), filter.len() - 1) == 0))
            test();
    }
}

App::function_t App::functions[APP_BUFFER_SIZE] = {};
const char* App::names[APP_BUFFER_SIZE] = {};
int App::cnt = 0;
