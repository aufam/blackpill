#include "periph/i2s.h"

namespace Project::Periph {

    void I2S::init(Callback::Function halfCBFn, void *halfCBArg, Callback::Function fullCBFn, void *fullCBArg) {
        setCallback(halfCBFn, halfCBArg, fullCBFn, fullCBArg);
        HAL_I2SEx_TransmitReceive_DMA(&hi2s,
                                      (uint16_t *)txBuffer.data(),
                                      (uint16_t *)rxBuffer.data(),
                                      Buffer::len() * nChannels);
    }

    void I2S::deinit() {
        HAL_I2S_DMAStop(&hi2s);
    }

    void I2S::setCallback(Callback::Function halfCBFn, void *halfCBArg, Callback::Function fullCBFn, void *fullCBArg) {
        halfCB.fn = halfCBFn; halfCB.arg = halfCBArg;
        fullCB.fn = fullCBFn; fullCB.arg = fullCBArg;
    }

} // namespace Project

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