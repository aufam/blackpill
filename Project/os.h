#ifndef PROJECT_OS_H
#define PROJECT_OS_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::OS {

    /// OS thread. requirements cmsis os v2
    struct Thread {
        typedef void (*Function) (void *arg);
        osThreadId_t id;
        constexpr Thread() : id(nullptr) {}

        void init(
                uint32_t stacksize,
                const char *name,
                osPriority_t prio,
                Function fn,
                void *arg = nullptr
        ) {
            if (id) return;
            osThreadAttr_t attr = {};
            attr.name = name;
            attr.stack_size = stacksize * 4; // word size
            attr.priority = prio;
            id = osThreadNew(fn, arg, &attr);
        }

        void deinit()   {
            if (id == nullptr) return;
            if (osThreadTerminate(id) == osOK) id = nullptr;
        }
    };

    /// static OS thread. requirements cmsis os v2
    template <size_t N>
    struct ThreadStatic : public Thread {
        uint32_t buffer[N];
        StaticTask_t controlblock;
        constexpr ThreadStatic() : Thread(), buffer{0}, controlblock{} {}

        void init(
                const char *name,
                osPriority_t prio,
                Function fn,
                void *arg = nullptr
        ) {
            if (id) return;
            osThreadAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlblock;
            attr.cb_size = sizeof(controlblock);
            attr.stack_mem = buffer;
            attr.stack_size = sizeof(buffer); // word size
            attr.priority = prio;
            id = osThreadNew(fn, arg, &attr);
        }
    };

    /// OS mutex. requirements cmsis os v2
    struct Mutex {
        osMutexId_t id;
        constexpr Mutex() : id(nullptr) {}

        void init(const char *name = nullptr) {
            if (id) return;
            osMutexAttr_t attr = {};
            attr.name = name;
            id = osMutexNew(&attr);
        }

        void deinit() {
            if (id == nullptr) return;
            if (osMutexDelete(id) == osOK) id = nullptr;
        }

        auto lock(uint32_t wait_ms = osWaitForever) const { return osMutexAcquire(id, wait_ms); }
        auto unlock() const                               { return osMutexRelease(id); }
    };

    /// static OS mutex. requirements cmsis os v2
    struct MutexStatic : public Mutex {
        StaticSemaphore_t controlblock;
        constexpr MutexStatic() : Mutex(), controlblock{} {}

        void init(const char *name = nullptr) {
            if (id) return;
            osMutexAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlblock;
            attr.cb_size = sizeof(controlblock);
            id = osMutexNew(&attr);
        }
    };

    /// OS timer. requirements cmsis os v2
    struct Timer {
        typedef void (*Function) (void *arg);
        osTimerId_t id;
        constexpr Timer() : id(nullptr) {}

        void init(
                Function fn,
                uint32_t interval,
                void *arg = nullptr,
                osTimerType_t type = osTimerPeriodic,
                const char *name = nullptr,
                int start_now = 1
        ) {
            if (id) return;
            osTimerAttr_t attr = {};
            attr.name = name;
            id = osTimerNew(fn, type, arg, &attr);
            if (start_now) osTimerStart(id, interval);
        }

        void deinit() {
            if (id == nullptr) return;
            if (osTimerDelete(id) == osOK) id = nullptr;
        }

        auto start(uint32_t interval) const   { return osTimerStart(id, interval); }
        auto stop() const                     { return osTimerStop(id); }
        auto isRunning() const               { return osTimerIsRunning(id); }
    };

    /// static OS timer. requirements cmsis os v2
    struct TimerStatic : public Timer {
        StaticTimer_t controlblock;
        constexpr TimerStatic() : Timer(), controlblock{} {}

        void init(
                Function fn,
                uint32_t interval,
                void *arg = nullptr,
                osTimerType_t type = osTimerPeriodic,
                const char *name = nullptr,
                int start_now = 1
        ) {
            if (id) return;
            osTimerAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlblock;
            attr.cb_size = sizeof(controlblock);
            id = osTimerNew(fn, type, arg, &attr);
            if (start_now) osTimerStart(id, interval);
        }
    };

    /// OS queue. requirements cmsis os v2
    template <class T>
    struct Queue {
        typedef T Type;
        osMessageQueueId_t id;
        constexpr Queue() : id(nullptr) {}

        void init(int32_t max_len, const char *name = nullptr) {
            if (id) return;
            osMessageQueueAttr_t attr = {};
            attr.name = name;
            id = osMessageQueueNew(max_len, sizeof(T), &attr);
        }

        void deinit() {
            if (id == nullptr) return;
            if (osMessageQueueDelete(id) == osOK) id = nullptr;
        }

        /// push an item to the queue
        /// @param[in] item the item
        /// @param[out] wait_ms wait in ms
        /// @param[in] prio priority level
        /// @retval osStatus_t, see cmsis_os2.h
        auto push(const T &item, uint32_t wait_ms = 0, uint8_t prio = 0) {
            return osMessageQueuePut(id, &item, prio, wait_ms);
        }
        /// pop an item from the queue
        /// @param[out] item the item
        /// @param[in] wait_ms wait in ms
        /// @param[out] prio priority level
        /// @retval osStatus_t, see cmsis_os2.h
        auto pop(T &item, uint32_t wait_ms = 0, uint8_t *prio = nullptr) {
            return osMessageQueueGet(id, &item, prio, wait_ms);
        }
        auto clear()        { return osMessageQueueReset(id); }
        auto getSpace()     { return osMessageQueueGetSpace(id); }
        auto getCount()     { return osMessageQueueGetCount(id); }
        auto getCapacity()  { return osMessageQueueGetCapacity(id); }
        auto getItemSize()  { return osMessageQueueGetMsgSize(id); }

        Queue &operator << (const T &item)  { push(item); return *this; }
        Queue &operator >> (T &item)        { pop(item); return *this; }
        explicit operator bool ()           { return getCount() > 0; }
    };

    /// static OS queue. requirements cmsis os v2
    template <class T, size_t N>
    struct QueueStatic : public Queue<T> {
        T buffer[N];
        StaticQueue_t controlblock;
        constexpr QueueStatic() : Queue<T>(), buffer{}, controlblock{} {}

        void init(const char *name = nullptr) {
            if (this->id) return;
            osMessageQueueAttr_t attr = {};
            attr.name = name;
            attr.cb_mem = &controlblock;
            attr.cb_size = sizeof(controlblock);
            attr.mq_mem = buffer;
            attr.mq_size = sizeof(buffer);
            this->id = osMessageQueueNew(N, sizeof(T), &attr);
        }
    };

    template <class T>
    struct LinkedList {
        struct Node {
            T item;
            Node *next;
        };
        Node *head;
        constexpr LinkedList() : head(nullptr) {}

        Node *tail() {
            auto node = head;
            if (node == nullptr) return node;
            for (; node->next != nullptr; node = node->next);
            return node;
        }

        size_t len() {
            size_t cnt = 0;
            for (auto node = head; node != nullptr; node = node->next) cnt++;
            return cnt;
        }

        int push(const T &item) {
            auto node = (Node *) pvPortMalloc(sizeof(Node));
            if (node == nullptr) return osError;
            node->item = item;
            node->next = nullptr;
            if (head == nullptr) head = node;
            else tail()->next = node;
            return osOK;
        }

        int pop(T &item) {
            auto node = head;
            if (node == nullptr) return osError;
            item = node->item;
            head = node->next;
            vPortFree(node);
            return osOK;
        }

        void clear() {
            for (T dummy; head != nullptr; ) pop(dummy);
        }

        LinkedList<T> &operator << (const T &item) { push(item); return *this; }
        LinkedList<T> &operator >> (T &item)       { pop(item); return *this; }
        explicit operator bool ()                  { return head != nullptr;}

        T operator [](size_t index) {
            auto node = head;
            if (node == nullptr) return T{};
            for (size_t i = 0; i < index; i++) {
                if (node == nullptr) return T{};
                node = node->next;
            }
            return node->item;
        }
    };

} // namespace Project

#endif // PROJECT_OS_H