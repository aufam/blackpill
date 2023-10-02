#include "project.h"
#include "periph/usb.h"
#include "etl/string.h"

#include "periph/all.h"

using namespace Project;
using namespace Project::etl::literals;

void project_init() {
    periph::usb.rxCallback = [] (const uint8_t* buf, size_t len) {
        periph::usb.transmit({buf, len}); // echo
        periph::usb.rxBuffer[len] = '\0'; // manually add string terminator

        auto& blink = etl::string_cast(blinkSymbols);
        auto& str = etl::string_cast<periph::USBD::Buffer::size()>(buf);

        blink = str;
    };
}
