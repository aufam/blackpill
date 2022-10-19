#ifndef PROJECT_PERIPH_EXTI_H
#define PROJECT_PERIPH_EXTI_H

#include "main.h"

namespace Project::Periph {

    /// external interrupt class
    struct Exti {
        /// callback function class
        struct Callback {
            typedef void (*Function)(void *);
            Function fn;
            void *arg;
        };

        Callback callbacks[16] = {}; ///< list of callback function of every GPIO pin
        inline static const uint32_t debounceDelay = 250; ///< button debounce delay
        constexpr Exti() = default;

        /// set exti callback of given GPIO pin
        /// @param pin GPIO_PIN_X see stm32fXxx_hal_gpio.h
        /// @param fn function pointer
        /// @param arg function argument
        void setCallback(uint16_t pin, Callback::Function fn = nullptr, void *arg = nullptr);
    };

    inline Exti exti;

} // namespace Project


#endif // PROJECT_PERIPH_EXTI_H