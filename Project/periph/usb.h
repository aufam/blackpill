#ifndef PROJECT_PERIPH_USB_H
#define PROJECT_PERIPH_USB_H

#include "usbd_cdc_if.h"
#include "etl/array.h"

namespace Project::Periph {

    /// USB peripheral class
    struct USB {
        /// callback function class
        struct Callback {
            typedef void (*Function)(void *, size_t);
            Function fn;
            void *arg;
        };
        using Buffer = etl::Array<uint8_t, APP_RX_DATA_SIZE>; ///< USB rx buffer type definition

        Callback rxCallback = {};
        Callback txCallback = {};
        Buffer &rxBuffer;
        constexpr explicit USB(Buffer &rxBuffer) : rxBuffer(rxBuffer) {}

        /// set rx callback
        /// @param rxCBFn receive callback function pointer
        /// @param rxCBArg receive callback function argument
        void setRxCallback(Callback::Function rxCBFn, void *rxCBArg = nullptr) {
            rxCallback.fn  = rxCBFn;
            rxCallback.arg = rxCBArg;
        }

        /// set tx callback
        /// @param txCBFn transmit callback function pointer
        /// @param txCBArg transmit callback function argument
        void setTxCallback(Callback::Function txCBFn, void *txCBArg = nullptr) {
            txCallback.fn  = txCBFn;
            txCallback.arg = txCBArg;
        }

        /// USB transmit non blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval USBD_StatusTypeDef (see usbd_def.h)
        int transmit(const void *buf, uint16_t len) {
            return CDC_Transmit_FS((uint8_t *)buf, len);
        }

        /// operator write string
        USB &operator << (const char *str) { transmit(str, strlen(str)); return *this; }
    };

    extern USB usb;

} // namespace Project


#endif // PROJECT_PERIPH_USB_H