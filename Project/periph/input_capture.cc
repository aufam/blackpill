#include "periph/input_capture.h"
#include "periph/encoder.h"

using namespace Project::periph;

static InputCapture* selector(TIM_HandleTypeDef *htim) {
    UNUSED(htim);
    return nullptr;
}

static Encoder* selectorEncoder(TIM_HandleTypeDef *htim) {
    if (htim->Instance == encoder4.htim.Instance) 
        return &encoder4;
    return nullptr;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    auto ic = selector(htim);
    if (ic) ic->callback();

    auto enc = selectorEncoder(htim);
    if (enc) enc->inputCaptureCallback();
}
