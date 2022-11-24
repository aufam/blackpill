#ifndef ETL_MUTEX_H
#define ETL_MUTEX_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    /// FreeRTOS mutex
    /// @note requires cmsis os v2
    struct Mutex {
        osMutexId_t id;
        StaticSemaphore_t controlBlock;
        constexpr Mutex() : id(nullptr), controlBlock{} {}

        /// initiate mutex
        /// @param name string name, default null
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t init(const char* name = nullptr) {
            if (id) return osError;
            osMutexAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            id = osMutexNew(&attr);
            return osOK;
        }

        /// deinit mutex
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osMutexDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        /// lock mutex
        /// @param waitMs wait in millisecond, default = @p osWaitForever
        /// @retval osStatusXxx
        osStatus_t lock(uint32_t waitMs = osWaitForever) { return osMutexAcquire(id, waitMs); }

        /// unlock mutex
        /// @retval osStatusXxx
        osStatus_t unlock() { return osMutexRelease(id); }
    };

    /// lock mutex when entering a scope and unlock when exiting
    struct MutexScope {
        Mutex& mutex;
        explicit MutexScope(Mutex& mutex) : mutex(mutex) { mutex.lock(); }
        ~MutexScope() { mutex.unlock(); }
    };
}

#endif //ETL_MUTEX_H
