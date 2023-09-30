#ifndef PERIPH_I2C_H
#define PERIPH_I2C_H

#include "Core/Inc/i2c.h"
#include "etl/function.h"

namespace Project::periph {

    /// I2C peripheral class
    /// @note requirements: event interrupt, tx DMA
    struct I2C {
        using Callback = etl::Function<void(), void*>; ///< callback function class

        I2C_HandleTypeDef &hi2c;    ///< I2C handler configured by cubeMX
        Callback txCallback = {};

        /// default constructor
        constexpr explicit I2C(I2C_HandleTypeDef &hi2c) : hi2c(hi2c) {}

        I2C(const I2C&) = delete; ///< disable copy constructor
        I2C& operator=(const I2C&) = delete;  ///< disable copy assignment

        void init() {}

        void deinit() {}

        /// set tx callback
        /// @param fn tx callback function
        /// @param ctx tx callback function argument
        template <typename Fn, typename Ctx>
        void setTxCallback(Fn&& fn, Ctx* ctx) { txCallback = Callback(etl::forward<Fn>(fn), ctx); }

        /// set tx callback
        /// @param fn tx callback function
        template <typename Fn>
        void setTxCallback(Fn&& fn) { txCallback = etl::forward<Fn>(fn); }

        /// I2C write blocking
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int writeBlocking(uint16_t deviceAddr,              ///< device destination address
                          uint16_t memAddr,                 ///< memory address
                          const uint8_t *buf,               ///< pointer to data buffer
                          uint16_t len,                     ///< buffer length
                          uint32_t timeout = HAL_MAX_DELAY  ///< in ms
        ) {
            while (hi2c.State != HAL_I2C_STATE_READY);
            return HAL_I2C_Mem_Write(&hi2c, deviceAddr, memAddr, 1, const_cast<uint8_t*>(buf), len, timeout);
        }

        /// I2C write non blocking
        /// @param deviceAddr device destination address
        /// @param memAddr memory address
        /// @param buf data buffer, either memory-fixed buffer or temporary buffer (max. 4 bytes)
        /// @param len buffer length
        /// @retval HAL_StatusTypeDef (see stm32fXxx_hal_def.h) or osStatus_t (cmsis_os2.h)
        int write(uint16_t deviceAddr, uint16_t memAddr, const uint8_t *buf, uint16_t len) {
            return HAL_I2C_Mem_Write_DMA(&hi2c, deviceAddr, memAddr, 1, const_cast<uint8_t*>(buf), len);
        }

        /// I2C read blocking
        /// @retval HAL_StatusTypeDef. see stm32fXxx_hal_def.h
        int readBlocking(uint16_t deviceAddr,               ///< destination address
                         uint16_t memAddr,                  ///< memory address
                         const uint8_t *buf,                      ///< data buffer
                         uint16_t len,                      ///< buffer length
                         uint32_t timeout = HAL_MAX_DELAY   ///< in ms
        ) {
            while (hi2c.State != HAL_I2C_STATE_READY);
            return HAL_I2C_Mem_Read(&hi2c, deviceAddr, memAddr, 1, const_cast<uint8_t*>(buf), len, timeout);
        }
    };

    inline I2C i2c1 { hi2c1 };

} // namespace Project


#endif // PERIPH_I2C_H