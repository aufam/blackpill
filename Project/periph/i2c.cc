#include "periph/i2c.h"

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    using namespace Project::Periph;
    I2C *i2c;
    if (hi2c->Instance == i2c1.hi2c.Instance) i2c = &i2c1;
    else return;

    I2C::Msg msg = {};
    if (i2c->txQueue.pop(msg) != osOK) return;
    uint8_t *buf = msg.len > sizeof(msg.bufTemp) ? msg.buf : msg.bufTemp;
    HAL_I2C_Mem_Write_DMA(
            &i2c->hi2c,
            msg.deviceAddr,
            msg.memAddr,
            1, buf, msg.len);
}
