#include "pwm.h"

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    PWM *pwm;
    if (htim->Instance == pwm2channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) pwm = &pwm2channel1;
    else return;

    auto &cb = pwm->halfCB;
    if (cb.fn) cb.fn(cb.arg);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    PWM *pwm;
    if (htim->Instance == pwm2channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) pwm = &pwm2channel1;
    else return;

    auto &cb = pwm->fullCB;
    if (cb.fn) cb.fn(cb.arg);
}
