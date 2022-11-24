#include "periph/usb.h"

extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

namespace Project::Periph {

    USB usb(*(USB::Buffer *) UserRxBufferFS);

}

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