#include "periph/exti.h"
#include "stm32f4xx_hal_gpio.h"
#include "os.h" // osKernelGetTickCount

namespace Project::Periph {

    void Exti::setCallback(uint16_t pin, Callback::Function fn, void *arg) {
        size_t index = 0;
        for (uint32_t b = 1; index < 16; index++)
            if ((b << index) & pin) {
                callbacks[index].fn = fn;
                callbacks[index].arg = arg;
            }
    }

} // namespace Project


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    using namespace Project::Periph;
    static uint16_t pinNow;
    static uint32_t timeNow;

    auto pinPrev = pinNow;
    auto timePrev = timeNow;
    timeNow = osKernelGetTickCount();
    pinNow = GPIO_Pin;
    if (timeNow - timePrev < Exti::debounceDelay && pinNow == pinPrev) return;

    size_t index = 0;
    for (uint32_t b = 1; index < 16; index++)
        if ((b << index) & GPIO_Pin) {
            auto &cb = exti.callbacks[index];
            if (cb.fn) cb.fn(cb.arg);
        }
}
