#ifndef ETL_QUEUE_H
#define ETL_QUEUE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/array.h"

namespace Project::etl {

    /// FreeRTOS queue interface
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    template <typename T>
    class QueueInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");

    protected:
        osMessageQueueId_t id; ///< queue pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticQueue_t*>(id)->uxDummy8; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticQueue_t*>(id)->uxDummy8; }

    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        /// empty constructor
        constexpr QueueInterface() : id(nullptr) {}

        /// construct from queue pointer
        explicit QueueInterface(osMessageQueueId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        QueueInterface(const QueueInterface& q) : id(q.id) {
            referenceCounterInc();
        }

        /// copy assignment
        QueueInterface& operator=(const QueueInterface& q) {
            if (id == q.id) return *this;
            detach();
            id = q.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        QueueInterface(QueueInterface&& q) : id(etl::exchange(q.id, nullptr)) {}

        /// move assignment
        QueueInterface& operator=(QueueInterface&& q) { 
            if (id == q.id) return *this;
            detach(); 
            id = etl::exchange(q.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~QueueInterface() { detach(); }
        
        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return reinterpret_cast<StaticQueue_t*>(id)->uxDummy8; }

        /// decrease reference counter and clean resource if reference counter is 0 
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0) 
                return osOK;

            return osMessageQueueDelete(etl::exchange(id, nullptr)); 
        }

        /// get queue pointer
        osMessageQueueId_t get() { return id; }

        /// push an item to the queue
        /// @param[in] item the item
        /// @param[in] timeout default timeImmediate
        /// @param[in] prio priority level, default 0 (lowest)
        /// @return osStatus
        /// @note can be called from ISR if timeout == 0
        osStatus_t push(const_reference item, etl::Time timeout = etl::time::immediate, uint8_t prio = 0) { 
            return osMessageQueuePut(id, &item, prio, timeout.tick); 
        }
        
        /// pop first item out from the queue
        /// @param[out] item first item from queue
        /// @param[in] timeout default timeImmediate
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @return osStatus
        /// @note can be called from ISR if timeout == 0
        osStatus_t pop(reference item, etl::Time timeout = etl::time::immediate, uint8_t* prio = nullptr) { 
            return osMessageQueueGet(id, &item, prio, timeout.tick); 
        }

        /// pop first item out from the queue
        /// @param[in] timeout wait in tick, default 0
        /// @param[out] prio pointer to priority level, default null (ignore)
        /// @return the first item
        /// @note can be called from ISR if timeout == 0
        value_type pop(etl::Time timeout = etl::time::immediate, uint8_t *prio = nullptr) {
            value_type item = {};
            pop(item, timeout, prio);
            return item;
        }

        /// get the capacity
        /// @note cannot be called from ISR
        uint32_t size() { return osMessageQueueGetCapacity(id); }

        /// get the item size in bytes
        /// @note cannot be called from ISR
        uint32_t itemSize() { return osMessageQueueGetMsgSize(id); }

        /// get the number of items
        /// @note can be called from ISR
        uint32_t len() { return osMessageQueueGetCount(id); }

        /// get the remaining space
        /// @note can be called from ISR
        uint32_t rem() { return osMessageQueueGetSpace(id); }

        /// reset the queue
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t clear() { return osMessageQueueReset(id); }

        /// return pointer of the data
        /// @note can be called from ISR
        iterator data() { 
            auto head = reinterpret_cast<StaticQueue_t*>(id);
            return reinterpret_cast<iterator>(head->pvDummy1[0]); 
        }

        /// return pointer of the first item
        /// @note can be called from ISR
        iterator begin() { return data(); }

        /// return pointer of one past the last item 
        /// @note can be called from ISR
        iterator end() { return begin() + len(); }

        /// return reference of first item 
        /// @note can be called from ISR
        /// @note the number of items has to be more than 0
        reference front() { return *begin(); }

        /// return reference of last item 
        /// @note can be called from ISR
        /// @note the number of items has to be more than 0
        reference back() { return *(begin() + (len() - 1)); }

        /// get the i-th item without removing from the queue
        /// @note can be called from ISR
        /// @note the number of items has to be more than 0
        reference operator[](int i) {
            auto l = len();
            if (l == 0) return *begin();
            if (i < 0) i = l + i; // allowing negative index
            return *(begin() + i);
        }

        /// push operator
        /// @param[in] item the first item
        /// @note can be called from ISR
        QueueInterface<T>& operator<<(const_reference item) { push(item); return *this; }

        /// pop operator
        /// @param[out] item the first item
        /// @note can be called from ISR
        QueueInterface<T>& operator>>(reference item) { pop(item); return *this; }

    };

    struct StaticQueueAttributes {
        const char* name = nullptr;
    };

    /// FreeRTOS static queue
    /// @tparam T item type
    /// @tparam N maximum number of items
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    template <class T, size_t N>
    class Queue : public QueueInterface<T> {
        StaticQueue_t controlBlock = {};
        Array<T, N> buffer = {};

    public:
        /// default constructor
        constexpr Queue() : QueueInterface<T>(nullptr) {}

        Queue(const Queue&) = delete; ///< disable copy constructor
        Queue(Queue&& t) = delete;    ///< disable move constructor

        Queue& operator=(const Queue&) = delete;  ///< disable copy assignment
        Queue& operator=(Queue&&) = delete;       ///< disable move assignment

        /// initiate queue
        /// @param attributes
        ///     - .name as null terminated string
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(StaticQueueAttributes attributes) {
            if (this->id) return osError;
            
            osMessageQueueAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.mq_mem = &buffer;
            attr.mq_size = sizeof(buffer);

            this->id = osMessageQueueNew(N, sizeof(T), &attr);
            this->referenceCounterInc();
            return osOK;
        }

        /// deinit queue
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return this->detach(); }
    };

    struct DynamicQueueAttributes {
        uint32_t capacity;
        const char* name = nullptr;
    };

    /// create dynamic queue 
    /// @tparam T item type
    /// @param attributes
    ///     - .capacity maximum number of items
    ///     - .name as null terminated string
    /// @return queue object
    /// @note cannot be called from ISR
    template <typename T>
    auto queue(DynamicQueueAttributes attributes) { 
        osMessageQueueAttr_t attr = {};
        attr.name = attributes.name;
        return QueueInterface<T>(osMessageQueueNew(attributes.capacity, sizeof(T), &attr)); 
    }

}

#endif //ETL_QUEUE_H
