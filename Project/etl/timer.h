#ifndef ETL_TIMER_H
#define ETL_TIMER_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    /// FreeRTOS timer
    /// @note requires cmsis os v2
    struct Timer {
        typedef void (*Function) (void *arg);
        osTimerId_t id;
        StaticTimer_t controlBlock;
        constexpr Timer() : id(nullptr), controlBlock{} {}

        /// initiate timer
        /// @param interval interval in ms
        /// @param fn function pointer
        /// @param arg function argument, default null
        /// @param type osTimerXxx, default @p osTimerPeriodic
        /// @param name string name, default null
        /// @param startNow true: start now (default), false: start later
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t init(uint32_t interval,
                        Function fn, void *arg = nullptr,
                        osTimerType_t type = osTimerPeriodic,
                        const char *name = nullptr,
                        bool startNow = true)
        {
            if (id) return osError;
            osTimerAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            id = osTimerNew(fn, type, arg, &attr);
            if (startNow) osTimerStart(id, interval);
            return osOK;
        }

        /// deinit timer
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osTimerDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        /// start timer
        /// @param interval interval in ms
        /// @retval osStatusXxx
        osStatus_t start(uint32_t interval) { return osTimerStart(id, interval); }

        /// start timer
        /// @retval osStatusXxx
        osStatus_t stop() { return osTimerStop(id); }

        /// check if this timer is running
        bool isRunning() { return osTimerIsRunning(id); }
    };

}

#endif //ETL_TIMER_H
