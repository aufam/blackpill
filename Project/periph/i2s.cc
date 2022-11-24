#include "periph/i2s.h"

void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    using namespace Project::Periph;
    I2S *i2s;
    if (hi2s->Instance == i2s2.hi2s.Instance) i2s = &i2s2;
    else return;

    auto &cb = i2s->halfCB;
    if (cb.fn) cb.fn(cb.arg);
}

void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s) {
    using namespace Project::Periph;
    I2S *i2s;
    if (hi2s->Instance == i2s2.hi2s.Instance) i2s = &i2s2;
    else return;

    auto &cb = i2s->fullCB;
    if (cb.fn) cb.fn(cb.arg);
}