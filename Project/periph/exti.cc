#include "periph/exti.h"
#include "etl/bit.h"
#include "cmsis_os2.h" // osKernelGetTickCount
#include "main.h"

using namespace Project;
using namespace Project::periph;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint16_t static pinNow;
    uint32_t static timeNow;

    auto pinPrev = pinNow;
    auto timePrev = timeNow;

    if (osKernelGetState() != osKernelRunning)
        return;
        
    timeNow = osKernelGetTickCount();
    pinNow = GPIO_Pin;
    if ((pinNow == pinPrev) && 
        (exti.isUsingDebounceFilter & GPIO_Pin) &&
        (timeNow - timePrev < Exti::debounceDelay)) 
        return;

    uint32_t b = 1;
    for (auto [i, callback] : etl::enumerate(exti.callbacks)) if ((b << i) & GPIO_Pin) {
        callback();
        exti.counters[i]++;
    }
}

void Exti::setCallback_(uint16_t pin, void (*fn)(void*), void *arg, bool useDebounceFilter) {
    exti.isUsingDebounceFilter = useDebounceFilter ? exti.isUsingDebounceFilter | pin : exti.isUsingDebounceFilter & (~pin);
    uint32_t b = 1;
    for (auto i : etl::range(16)) if ((b << i) & pin) {
        exti.callbacks[i] = Callback(fn, arg);
        exti.counters[i] = 0;
    }
}

uint32_t Exti::getCounter(uint16_t pin) { 
    return exti.counters[etl::count_trailing_zeros(pin)]; 
}

void Exti::setCounter(uint16_t pin, uint32_t cnt) { 
    uint32_t b = 1;
    for (auto i : etl::range(16)) if ((b << i) & pin)
        exti.counters[i] = cnt;
}

