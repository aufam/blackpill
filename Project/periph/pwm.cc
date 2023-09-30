#include "periph/pwm.h"

using namespace Project::periph;

static PWM* select(TIM_HandleTypeDef *htim) {
    if (htim->Instance == pwm2channel1.htim.Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) 
        return &pwm2channel1;
    
    return nullptr;
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
    auto pwm = select(htim);
    if (pwm == nullptr)
        return;

    pwm->halfCB();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    auto pwm = select(htim);
    if (pwm == nullptr)
        return;

    pwm->fullCB();
}
