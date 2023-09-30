#include "project.h"
#include "periph/usb.h"
#include "periph/bootloader.h"
#include "etl/string.h"

using namespace Project;

void project_init() {
    periph::usb.rxCallback = [] (const uint8_t* buf, size_t len) {
        periph::usb.transmit(buf, len); // echo
        periph::usb.rxBuffer[len] = '\0'; // manually add string terminator

        auto& blink = etl::string_cast(blinkSymbols);
        auto& str = etl::string_cast<periph::USBD::Buffer::size()>(buf);

        blink = str;
    };
}
