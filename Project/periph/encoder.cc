#include "encoder.h"

namespace Project::Periph {

    void Encoder::init(
            Callback::Function incrementCBFn, void *incrementCBArg,
            Callback::Function decrementCBFn, void *decrementCBArg)
    {
        setIncrementCB(incrementCBFn, incrementCBArg);
        setDecrementCB(decrementCBFn, decrementCBArg);
        HAL_TIM_Encoder_Start_IT(&htim, TIM_CHANNEL_ALL);
        timer.init(
                [](void *arg) {
                    auto encoder = (Encoder *) arg;
                    encoder->speed = (int16_t) (encoder->val - encoder->valPrev);
                    encoder->valPrev = encoder->val;
                }, 1000, this);
    }

    void Encoder::deinit() {
        HAL_TIM_Encoder_Stop_IT(&htim, TIM_CHANNEL_ALL);
        timer.deinit();
    }

} // namespace Project

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    Encoder *encoder;
    if (htim->Instance == encoder4.htim.Instance) encoder = &encoder4;
    else return;

    uint16_t counter = encoder->htim.Instance->CNT;
    auto &val = encoder->val;
    auto &inc = encoder->incrementCB;
    auto &dec = encoder->decrementCB;
    int cnt = counter / 4;
    if (inc.fn && cnt > val) inc.fn(inc.arg);
    if (dec.fn && cnt < val) dec.fn(dec.arg);
    val = (int16_t) cnt;
}