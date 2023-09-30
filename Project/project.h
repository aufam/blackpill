#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char blinkSymbols[16]; ///< blink symbols from default task. '0' is off, '1' is on
extern int blinkIsRunning; ///< blink symbols from default task. '0' is off, '1' is on
extern osThreadId_t defaultTaskHandle; ///< default task for led blink
void project_init(); ///< project initialization. should be added in main function

#ifdef __cplusplus
}
#endif

#endif // PROJECT_PROJECT_H
