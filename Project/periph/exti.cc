#include "periph/exti.h"
#include "cmsis_os2.h" // osKernelGetTickCount

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
