#ifndef PERIPH_PWM_H
#define PERIPH_PWM_H


#include "main.h"
#ifdef HAL_TIM_MODULE_ENABLED

#include "periph/config.h"

#include "Core/Inc/tim.h"
#include "etl/function.h"

namespace Project::periph {

    /// PWM generation.
    /// @note requires: TIMx PWM generation mode, TIMx global interrupt
    struct PWM {
        using Callback = etl::Function<void(), void*>; ///< callback function class
        inline static detail::UniqueInstances<PWM, 16> Instances;

        TIM_HandleTypeDef &htim;    ///< tim handler generated by cubeMX
        uint32_t channel;           ///< TIM_CHANNEL_x
        Callback halfCallback = {};
        Callback fullCallback = {};

        PWM(const PWM&) = delete;               ///< disable copy constructor
        PWM& operator=(const PWM&) = delete;    ///< disable copy assignment

        /// register this instance
        void init() {
            Instances.push(this);
        }

        struct InitArgs { uint32_t prescaler, period, pulse; };

        /// setup prescaler, period, and pulse, and register this instance
        /// @param args
        ///     - .prescaler set TIMx->PSC
        ///     - .period set TIMx->ARR
        ///     - .pulse set TIMx->CCRchannel
        void init(InitArgs args) {
            setPrescaler(args.prescaler);
            setPeriod(args.period);
            setPulse(args.pulse);
            init();
        }

        /// stop pwm and unregister this instance
        void deinit() { 
            stop(); 
            Instances.pop(this);
        }

        /// start pwm interrupt
        void start() { 
            #ifdef PERIPH_PWM_USE_IT
            HAL_TIM_PWM_Start_IT(&htim, channel); 
            #endif
            #ifdef PERIPH_PWM_USE_DMA
            HAL_TIM_PWM_Start_DMA(&htim, channel); 
            #endif
        }

        /// stop pwm interrupt
        void stop() { 
            #ifdef PERIPH_PWM_USE_IT
            HAL_TIM_PWM_Stop_IT(&htim, channel); 
            #endif
            #ifdef PERIPH_PWM_USE_DMA
            HAL_TIM_PWM_Stop_DMA(&htim, channel); 
            #endif
        }

        /// set TIMx->PSC
        void setPrescaler(uint16_t prescaler) const { htim.Instance->PSC = prescaler; }

        /// set TIMx->ARR
        void setPeriod(uint32_t period) const { htim.Instance->ARR = period; }

        /// set TIMx->CCRy
        void setPulse(uint32_t pulse) const {
            switch (channel) {
                case TIM_CHANNEL_1: htim.Instance->CCR1 = pulse; break;
                case TIM_CHANNEL_2: htim.Instance->CCR2 = pulse; break;
                case TIM_CHANNEL_3: htim.Instance->CCR3 = pulse; break;
                case TIM_CHANNEL_4: htim.Instance->CCR4 = pulse; break;
                default: break;
            }
        }

        /// get TIMx->PSC
        [[nodiscard]] uint16_t getPrescaler() const { return htim.Instance->PSC; }

        /// get TIMx->ARR
        [[nodiscard]] uint32_t getPeriod() const { return htim.Instance->ARR; }

        /// get TIMx->CCRy
        [[nodiscard]] uint32_t getPulse() const {
            switch (channel) {
                case TIM_CHANNEL_1: return htim.Instance->CCR1;
                case TIM_CHANNEL_2: return htim.Instance->CCR2;
                case TIM_CHANNEL_3: return htim.Instance->CCR3;
                case TIM_CHANNEL_4: return htim.Instance->CCR4;
                default: return 0;
            }
        }
    };
}

#endif // HAL_TIM_MODULE_ENABLED
#endif // PERIPH_PWM_H
