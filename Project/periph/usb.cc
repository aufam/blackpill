#include "periph/usb.h"

extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

namespace Project::Periph {

    void USB::setRxCallback(Callback::Function rxCBFn, void *rxCBArg) {
        rxCallback.fn  = rxCBFn;
        rxCallback.arg = rxCBArg;
    }

    void USB::setTxCallback(Callback::Function txCBFn, void *txCBArg) {
        txCallback.fn  = txCBFn;
        txCallback.arg = txCBArg;
    }

    int USB::transmit(const void *buf, uint16_t len) {
        return CDC_Transmit_FS((uint8_t *)buf, len);
    }

    USB usb(*(USB::Buffer *) UserRxBufferFS);

} // namespace Project

void CDC_ReceiveCplt_Callback(uint8_t *pbuf, uint32_t len) {
    (void) pbuf;
    using namespace Project::Periph;
    auto &cb = usb.rxCallback;
    if (cb.fn) cb.fn(cb.arg, len);
}

void CDC_TransmitCplt_Callback(uint8_t *pbuf, uint32_t len) {
    (void) pbuf;
    using namespace Project::Periph;
    auto &cb = usb.txCallback;
    if (cb.fn) cb.fn(cb.arg, len);
}