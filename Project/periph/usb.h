#ifndef PERIPH_USB_H
#define PERIPH_USB_H

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

        /// set rx callback
        /// @param fn receive callback function
        /// @param ctx receive callback function context
        template <typename Fn, typename Ctx>
        void setRxCallback(Fn&& fn, Ctx* ctx) { rxCallback = Callback(etl::forward<Fn>(fn), ctx); }

        /// set rx callback
        /// @param fn receive callback function
        template <typename Fn>
        void setRxCallback(Fn&& fn) { rxCallback = etl::forward<Fn>(fn); }

        /// set tx callback
        /// @param fn receive callback function
        /// @param ctx receive callback function context
        template <typename Fn, typename Ctx>
        void setTxCallback(Fn&& fn, Ctx* ctx) { txCallback = Callback(etl::forward<Fn>(fn), ctx); }

        /// set tx callback
        /// @param fn receive callback function
        template <typename Fn>
        void setTxCallback(Fn&& fn) { txCallback = etl::forward<Fn>(fn); }

        /// USB transmit non blocking
        /// @param buf data buffer
        /// @param len buffer length
        /// @retval @ref USBD_StatusTypeDef (see usbd_def.h)
        int transmit(const void *buf, uint16_t len) { return CDC_Transmit_FS((uint8_t *) buf, len); }

        /// write operator for etl::string
        template <size_t N>
        USBD& operator<<(const etl::String<N>& str) { transmit(str.data(), str.len()); return *this; }

        /// write operator for traditional string
        USBD& operator<<(const char *str) { transmit(str, strlen(str)); return *this; }
    };

    extern USBD usb;

} // namespace Project

#endif // PERIPH_USB_H