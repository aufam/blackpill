#include <apps/app.h>
#include <apps/ip_config.h>
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

extern mac_t eeprom_read_mac();
extern ip_t eeprom_read_ip();
extern ip_t eeprom_read_sn();
extern ip_t eeprom_read_gw();
extern ip_t eeprom_read_dns();

extern "C" void project_init() {
    etl::task::init();
    delameta_stm32_hal_init();

    auto mac = eeprom_read_mac();
    auto ip  = eeprom_read_ip();
    auto sn  = eeprom_read_sn();
    auto gw  = eeprom_read_gw();
    auto dns = eeprom_read_dns();
    delameta_stm32_hal_wizchip_set_net_info(
        mac.value.data(), ip.value.data(), sn.value.data(), gw.value.data(), dns.value.data()
    );

    delameta_stm32_hal_wizchip_init();
    App::run();
}

extern "C" void panic(const char* msg) {
    etl::task::terminate(); // terminate all tasks
    taskDISABLE_INTERRUPTS();
	__disable_irq();

    const uint8_t title[] = "PANIC: ";
    const uint8_t line_feed = '\n';
    const auto msg_len = ::strlen(msg);
    extern UART_HandleTypeDef huart1;

    for (;;) {
        HAL_UART_Transmit(&huart1, title, sizeof(title) - 1, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, (const uint8_t*)msg, msg_len, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart1, &line_feed, 1, HAL_MAX_DELAY);
        for (int i = 0; i < 10'000'000; ++i);
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
