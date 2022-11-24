#ifndef ETL_QUEUE_H
#define ETL_QUEUE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    /// FreeRTOS message queue
    /// @tparam T item type
    /// @tparam N maximum number of item
    /// @note requires cmsis os v2
    template <class T, size_t N>
    struct Queue {
        typedef T Type;
        osMessageQueueId_t id;
        T buffer[N];
        StaticQueue_t controlBlock;
        constexpr Queue() : id(nullptr), buffer{}, controlBlock{} {}

        /// initiate mutex
        /// @param name string name, default null
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t init(const char *name = nullptr) {
            if (id) return osError;
            osMessageQueueAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.mq_mem = buffer;
            attr.mq_size = sizeof(buffer);
            id = osMessageQueueNew(N, sizeof(T), &attr);
            return osOK;
        }

        /// deinit mutex
        /// @retval @p osOK: success, @p osError: failed (already initiated)
        osStatus_t deinit() {
            if (id == nullptr) return osError;
            if (osMessageQueueDelete(id) == osOK) id = nullptr;
            return osOK;
        }

        /// push an item to the queue
        /// @param[in] item the item
        /// @param[out] waitMs wait in ms, default 0
        /// @param[in] prio priority level, default 0 (lowest)
        /// @retval osStatusXxx
        osStatus_t push(const T &item, uint32_t waitMs = 0, uint8_t prio = 0) {
            return osMessageQueuePut(id, &item, prio, waitMs);
        }

        /// pop first item from the queue
        /// @param[out] item the item
        /// @param[in] waitMs wait in ms, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @retval osStatusXxx
        osStatus_t pop(T &item, uint32_t waitMs = 0, uint8_t *prio = nullptr) {
            return osMessageQueueGet(id, &item, prio, waitMs);
        }

        /// clear all items inside this queue
        osStatus_t clear() { return osMessageQueueReset(id); }

        /// remaining space in this queue
        auto rem() { return osMessageQueueGetSpace(id); }

        /// number of items in this queue
        auto len() { return osMessageQueueGetCount(id); }

        /// maximum number of item in this queue
        auto maxLen() { return osMessageQueueGetCapacity(id); }

        /// item size in bytes
        auto itemSize()  { return osMessageQueueGetMsgSize(id); }

        /// push operator, no wait
        Queue &operator << (const T &item) { push(item); return *this; }

        /// pop operator, no wait
        Queue &operator >> (T &item) { pop(item); return *this; }

        explicit operator bool () { return len() > 0; }
    };

}

#endif //ETL_QUEUE_H
