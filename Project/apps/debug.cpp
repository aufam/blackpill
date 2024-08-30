#include <fmt/format.h>
#include <delameta/debug.h>
#include <usart.h>

#define huart &huart1

using namespace Project;

static const char* get_file_name_only(const char* file) {
    const char* filename = strrchr((const char*)file, '/');
    if (!filename) {
        filename = strrchr((const char*)file, '\\');  // check for windows-style paths
    }
    return filename ? filename + 1 : (const char*)file; 
}

[[override_weak]]
void delameta::info(const char* file, int line, const std::string& msg) {
    auto text = fmt::format("{}:{} INFO: {}\n", get_file_name_only(file), line, msg);
    HAL_UART_Transmit(huart, (const uint8_t*)text.c_str(), text.size(), HAL_MAX_DELAY);
}

[[override_weak]]
void delameta::warning(const char* file, int line, const std::string& msg) {
    auto text = fmt::format("{}:{} WARNING: {}\n", get_file_name_only(file), line, msg);
    HAL_UART_Transmit(huart, (const uint8_t*)text.c_str(), text.size(), HAL_MAX_DELAY);
}
