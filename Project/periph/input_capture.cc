#include "periph/input_capture.h"
#include "periph/encoder.h"

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    using namespace Project::Periph;
    Encoder *encoder = nullptr;
    if (htim->Instance == encoder4.htim.Instance) encoder = &encoder4;
    /* add another Encoder instance here */

    if (encoder != nullptr) {
        encoder->inputCaptureCallback();
        return;
    }

    /* add InputCapture instance here*/

}
