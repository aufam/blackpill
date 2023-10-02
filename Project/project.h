#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char blinkSymbols[16];           ///< blink symbols from default task. '0' is off, '1' is on
extern int blinkIsRunning;              ///< blink thread running flag. set to 0 to stop the thread
extern osThreadId_t defaultTaskHandle;  ///< default task for led blink
void project_init();                    ///< project initialization. should be added in main function

#ifdef __cplusplus
}

#include "periph/all.h"

// periph instances
namespace Project::periph {
    inline Encoder encoder4 { .htim=htim4 };
    inline I2C i2c1 { .hi2c=hi2c1 };
    inline I2S i2s2 { .hi2s=hi2s2 };
    inline PWM pwm2channel1 { .htim=htim2, .channel=TIM_CHANNEL_1 };
    inline UART uart2 { .huart=huart2 };
}

#endif

#endif // PROJECT_PROJECT_H
