#ifndef PERIPH_USB_H
#define PERIPH_USB_H

#include "main.h"
#ifdef HAL_PCD_MODULE_ENABLED

#include "usbd_cdc_if.h"
#include "etl/array.h"
#include "etl/string.h"
#include "etl/function.h"

namespace Project::periph {

    /// USB peripheral class
    struct USBD {
        using Callback = etl::Function<void(const uint8_t*, size_t), void*>; ///< callback function class
        using Buffer = etl::Array<uint8_t, APP_RX_DATA_SIZE>;                ///< USB rx buffer classs

        Callback rxCallback = {};   ///< rx callback function
        Callback txCallback = {};   ///< tx callback function
        Buffer &rxBuffer;           ///< reference to USB rx buffer

        /// default constructor
        constexpr explicit USBD(Buffer &rxBuffer) : rxBuffer(rxBuffer) {}

        USBD(const USBD&) = delete; ///< disable copy constructor
        USBD& operator=(const USBD&) = delete;  ///< disable move constructor

        struct TransmitArgs { const void *buf; size_t len; };

        /// USB transmit non blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval @ref USBD_StatusTypeDef (see usbd_def.h)
        int transmit(TransmitArgs args) { return CDC_Transmit_FS((uint8_t*) args.buf, args.len); }

        /// write operator for etl::string
        template <size_t N>
        USBD& operator<<(const etl::String<N>& str) { 
            transmit({.buf=str.data(), .len=str.len()}); 
            return *this; 
        }

        /// write operator for traditional string
        USBD& operator<<(const char *str) { 
            transmit({.buf=str, .len=strlen(str)}); 
            return *this; 
        }
    };

    extern USBD usb;

} // namespace Project

#endif // HAL_PCD_MODULE_ENABLED
#endif // PERIPH_USB_H