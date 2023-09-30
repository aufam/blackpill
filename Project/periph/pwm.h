#ifndef PERIPH_PWM_H
#define PERIPH_PWM_H

#include "Core/Inc/tim.h"
#include "etl/function.h"

namespace Project::periph {

    /// PWM generation.
    /// @note requires: TIMx PWM generation mode, TIMx global interrupt
    struct PWM{
        using Callback = etl::Function<void(), void*>; ///< callback function class

        TIM_HandleTypeDef &htim; ///< tim handler generated by cubeMX
        uint32_t channel;        ///< TIM_CHANNEL_x
        Callback halfCB = {};
        Callback fullCB = {};

        /// default constructor
        constexpr PWM(TIM_HandleTypeDef &htim, uint32_t channel) : htim(htim), channel(channel) {}

        PWM(const PWM&) = delete; ///< disable copy constructor
        PWM& operator=(const PWM&) = delete;  ///< disable move constructor

        /// set prescaler, period, pulse
        /// @param prescaler set TIMx->PSC
        /// @param period set TIMx->ARR
        /// @param pulse set TIMx->CCRchannel
        void init(uint16_t prescaler, uint32_t period, uint32_t pulse) {
            setPrescaler(prescaler);
            setPeriod(period);
            setPulse(pulse);
        }

        struct InitParams { uint32_t prescaler, period, pulse; };
        void init(InitParams params) {
            init(params.prescaler, params.period, params.pulse);
        }

        /// stop pwm
        void deinit() { stop(); }

        /// set half callback
        /// @param fn half callback function
        /// @param ctx half callback function context
        template <typename Fn, typename Ctx>
        void setHalfCB(Fn&& fn, Ctx* ctx) { halfCB = Callback(etl::forward<Fn>(fn), ctx); }
        
        /// set half callback
        /// @param fn half callback function
        template <typename Fn>
        void setHalfCB(Fn&& fn) { halfCB = etl::forward<Fn>(fn); }

        /// set full callback
        /// @param fn full callback function
        /// @param ctx full callback function context
        template <typename Fn, typename Ctx>
        void setFullCB(Fn&& fn, Ctx* ctx) { fullCB = Callback(etl::forward<Fn>(fn), ctx); }
        
        /// set full callback
        /// @param fn full callback function
        template <typename Fn>
        void setFullCB(Fn&& fn) { fullCB = etl::forward<Fn>(fn); }

        /// start pwm interrupt
        void start() { HAL_TIM_PWM_Start_IT(&htim, channel); }

        /// stop pwm interrupt
        void stop() { HAL_TIM_PWM_Stop_IT(&htim, channel); }

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

    /// PWM generation timer 2 channel 1
    inline PWM pwm2channel1 {htim2, TIM_CHANNEL_1 };

}

#endif //PERIPH_PWM_H
