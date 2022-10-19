#include "periph/i2c.h"
#include <cstring> // memset

namespace Project::Periph{

    void I2C::init() {
        txQueue.init();
    }

    void I2C::deinit() {
        txQueue.deinit();
    }

    int I2C::writeBlocking(uint16_t deviceAddr, uint16_t memAddr, uint8_t *buf, uint16_t len) {
        while (hi2c.State != HAL_I2C_STATE_READY);
        return HAL_I2C_Mem_Write(
                &hi2c,
                deviceAddr,
                memAddr,
                1, buf, len,
                HAL_MAX_DELAY);
    }

    int I2C::write(uint16_t deviceAddr, uint16_t memAddr, uint8_t *buf, uint16_t len) {
        if (hi2c.State == HAL_I2C_STATE_READY && hi2c.hdmatx->State == HAL_DMA_STATE_READY) {
            if (!txQueue) {
                // transmit immediately if the queue is empty
                return HAL_I2C_Mem_Write_DMA(
                        &hi2c,
                        deviceAddr,
                        memAddr,
                        1, buf, len);
            }

            // transmit data from queue if any
            Msg msg = {};
            txQueue >> msg;
            uint8_t *_buf = msg.len > sizeof(msg.bufTemp) ? msg.buf : msg.bufTemp;
            HAL_I2C_Mem_Write_DMA(
                    &hi2c,
                    msg.deviceAddr,
                    msg.memAddr,
                    1, _buf, msg.len);
        }

        // i2c is busy, push to the queue, it will be handled in complete callback function
        Msg msg = {};
        msg.deviceAddr = deviceAddr;
        msg.memAddr = memAddr;
        msg.len = len;
        msg.buf = buf;
        if (len <= sizeof(msg.bufTemp)) memcpy(msg.bufTemp, buf, len);
        return txQueue.push(msg);
    }

} // namespace Project

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
