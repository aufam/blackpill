#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#ifdef __cplusplus
#include "dsp/buffer.h"
#include "dsp/types.h"
#include "dsp/vco.h"
#include "oled/oled.h"
#include "periph/encoder.h"
#include "periph/exti.h"
#include "periph/i2c.h"
#include "periph/i2s.h"
#include "periph/pwm.h"
#include "periph/rtc.h"
#include "periph/uart.h"
#include "periph/usb.h"
#include "audio.h"
#include "buzzer.h"
#include "codec.h"
#include "fstring.h"
#include "os.h"
extern "C" {
#endif

void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_PROJECT_H
