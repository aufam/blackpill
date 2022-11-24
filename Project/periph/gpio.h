#ifndef PROJECT_PERIPH_PIN_H
#define PROJECT_PERIPH_PIN_H

#include "main.h"

namespace Project::Periph {

    struct GPIO {
        enum { activeLow, activeHigh };

        GPIO_TypeDef *port; /// GPIOx
        uint16_t pin; /// GPIO_PIN_x
        bool activeMode; /// activeLow or activeHigh

        constexpr GPIO(GPIO_TypeDef *port, uint16_t pin, bool activeMode)
        : port(port)
        , pin(pin)
        , activeMode(activeMode) {}

        static void init(GPIO_TypeDef *port,
                         uint32_t pin,
                         uint32_t mode,
                         uint32_t pull = GPIO_NOPULL,
                         uint32_t speed = GPIO_SPEED_FREQ_LOW) {
            GPIO_InitTypeDef gpioInitStruct = { .Pin = pin,
                                                .Mode = mode,
                                                .Pull = pull,
                                                .Speed = speed };
            HAL_GPIO_Init(port, &gpioInitStruct);
        }

        void init(uint32_t mode, uint32_t pull = GPIO_NOPULL, uint32_t speed = GPIO_SPEED_FREQ_LOW) const {
            init(port, pin, mode, pull, speed);
        }

        /// write pin high (true) or low (false)
        void write(bool highLow) const {
            HAL_GPIO_WritePin(port, pin, highLow ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }

        void toggle() const {
            HAL_GPIO_TogglePin(port, pin);
        }

        [[nodiscard]] bool read() const {
            bool res = HAL_GPIO_ReadPin(port, pin);
            return !(res ^ activeMode);
        }

        void on() const { write(activeMode); }

        void off() const { write(!activeMode); }
    };

} // Periph

#endif //PROJECT_PERIPH_PIN_H
