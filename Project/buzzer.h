//
// Created by aufa on 07/09/22.
//

#ifndef PROJECT_BUZZER_H
#define PROJECT_BUZZER_H

#include "periph/pwm.h"

namespace Project {

    /// Buzzer class. requirements: TIMx PWM generation, global interrupt
    struct Buzzer {
        using PWM = Periph::PWM;
        using Callback = Periph::PWM::Callback;

        PWM &pwm; ///< PWM instance. should be declared in pwm.h
        uint32_t channel; ///< TIM_CHANNEL_x
        uint32_t nPulse; ///< number of pulse
        uint32_t cnt; ///< PWM counter
        constexpr Buzzer(Periph::PWM &pwm, uint32_t channel, uint32_t nPulse)
            : pwm(pwm)
            , channel(channel)
            , nPulse(nPulse)
            , cnt(0)
        {}

        /// initiation
        /// @param prescaler TIMx->PSC
        /// @param period TIMx->ARR
        /// @param pulse TIMx->CCRy
        void init(uint16_t prescaler, uint32_t period, uint32_t pulse);
        void start() { pwm.start(channel); }
        void stop() { pwm.stop(channel); }
        void setNPulse(int newNPulse) { nPulse = newNPulse; }
    };

} // Project


#endif //PROJECT_BUZZER_H
