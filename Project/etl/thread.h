#ifndef ETL_THREAD_H
#define ETL_THREAD_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

#ifndef ETL_THREAD_DEFAULT_BUFFER_WORDS
#define ETL_THREAD_DEFAULT_BUFFER_WORDS 128
#endif

namespace Project::etl {

    /// FreeRTOS thread
    /// @tparam N buffer length (in words)
    /// @note requires cmsis os v2
    template <size_t N = 128>
    struct Thread {
        typedef void (* Function) (void* arg);
        osThreadId_t id;
        uint32_t buffer[N];
        StaticTask_t controlBlock;
        constexpr Thread() : id(nullptr), buffer{}, controlBlock{} {}

        /// initiate thread
        /// @param fn function pointer
        /// @param arg function argument, default null
        /// @param prio osPriorityXxx, default @p osPriorityNormal
        /// @param name string name, default null
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t init(Function fn, void* arg = nullptr,
                        osPriority_t prio = osPriorityNormal,
                        const char* name = nullptr)
        {
            if (id) return osError;
            osThreadAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.stack_mem = buffer;
            attr.stack_size = sizeof(buffer); // in byte
            attr.priority = prio;
            id = osThreadNew(fn, arg, &attr);
            return osOK;
        }

        /// terminate thread
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osThreadTerminate(id) == osOK) id = nullptr;
            return osOK;
        }
    };

}

#endif //ETL_THREAD_H
