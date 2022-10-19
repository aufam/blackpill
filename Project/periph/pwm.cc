#include "pwm.h"

namespace Project::Periph {

    void PWM::init(uint16_t prescaler, uint32_t period, uint32_t pulse, uint32_t channel,
                   Callback::Function halfCBFn, void *halfCBArg,
                   Callback::Function fullCBFn, void *fullCBArg) {
        setPrescaler(prescaler);
        setPeriod(period);
        setPulse(pulse, channel);
        setCallback(halfCBFn, halfCBArg, fullCBFn, fullCBArg);
    }

    void PWM::setCallback(
            Callback::Function halfCBFn, void *halfCBArg,
            Callback::Function fullCBFn, void *fullCBArg) {
        halfCB.fn = halfCBFn; halfCB.arg = halfCBArg;
        fullCB.fn = fullCBFn; fullCB.arg = fullCBArg;
    }

    void PWM::start(uint32_t channel) { HAL_TIM_PWM_Start_IT(&htim, channel); }

    void PWM::stop(uint32_t channel) { HAL_TIM_PWM_Stop_IT(&htim, channel); }

    void PWM::setPrescaler(uint16_t prescaler) const { htim.Instance->PSC = prescaler; }

    void PWM::setPeriod(uint32_t period) const { htim.Instance->ARR = period; }

    void PWM::setPulse(uint32_t pulse, uint32_t channel) const {
        switch (channel) {
            case TIM_CHANNEL_1: htim.Instance->CCR1 = pulse; break;
            case TIM_CHANNEL_2: htim.Instance->CCR2 = pulse; break;
            case TIM_CHANNEL_3: htim.Instance->CCR3 = pulse; break;
            case TIM_CHANNEL_4: htim.Instance->CCR4 = pulse; break;
            default: break;
        }
    }

    uint16_t PWM::getPrescaler() const { return htim.Instance->PSC; }

    uint32_t PWM::getPeriod() const { return htim.Instance->ARR; }

    uint32_t PWM::getPulse(uint32_t channel) const {
        switch (channel) {
            case TIM_CHANNEL_1: return htim.Instance->CCR1;
            case TIM_CHANNEL_2: return htim.Instance->CCR2;
            case TIM_CHANNEL_3: return htim.Instance->CCR3;
            case TIM_CHANNEL_4: return htim.Instance->CCR4;
            default: return 0;
        }
    }

}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    PWM *pwm;
    if (htim->Instance == pwm2.htim.Instance) pwm = &pwm2;
    else return;

    auto &cb = pwm->halfCB;
    if (cb.fn) cb.fn(cb.arg);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    PWM *pwm;
    if (htim->Instance == pwm2.htim.Instance) pwm = &pwm2;
    else return;

    auto &cb = pwm->fullCB;
    if (cb.fn) cb.fn(cb.arg);
}
