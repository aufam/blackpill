#ifndef ETL_SEMAPHORE_H
#define ETL_SEMAPHORE_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"
#include "etl/time.h"

namespace Project::etl {

    /// FreeRTOS semaphore interface
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    class SemaphoreInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");

    protected:
        osSemaphoreId_t id; ///< semaphore pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }

    public:
        /// empty constructor
        constexpr SemaphoreInterface() : id(nullptr) {}

        /// construct from semaphore pointer
        explicit SemaphoreInterface(osSemaphoreId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        SemaphoreInterface(const SemaphoreInterface& s) : id(s.id) {
            referenceCounterInc();
        }

        /// copy assignment
        SemaphoreInterface& operator=(const SemaphoreInterface& s) {
            if (id == s.id) return *this;
            detach();
            id = s.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        SemaphoreInterface(SemaphoreInterface&& s) : id(etl::exchange(s.id, nullptr)) {}

        /// move assignment
        SemaphoreInterface& operator=(SemaphoreInterface&& s) { 
            if (id == s.id) return *this;
            detach(); 
            id = etl::exchange(s.id, nullptr); 
            return *this; 
        }

        /// default destructor
        ~SemaphoreInterface() { detach(); }
        
        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return id ? reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8 : 0; }

        /// get semaphore pointer
        osSemaphoreId_t get() { return id; }

        /// delete resource and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0) 
                return osOK;
            
            return osSemaphoreDelete(etl::exchange(id, nullptr)); 
        }

        /// name as null terminated string
        const char* getName() { return osSemaphoreGetName(id); }  

        /// acquire semaphore token, token counter will be decreased by one
        /// @param timeout default = timeInfinite
        /// @return osStatus
        /// @note can be called from ISR if timeout == time::immediate
        osStatus_t acquire(etl::Time timeout = etl::time::infinite) { return osSemaphoreAcquire(id, timeout.tick); }

        /// release semaphore token, token counter will be increased by one
        /// @return osStatus
        /// @note can be called from ISR
        osStatus_t release() { return osSemaphoreRelease(id); }

        /// get token counter
        /// @note can be called from ISR
        uint32_t tokenCount() { return osSemaphoreGetCount(id); }

        /// release operator
        SemaphoreInterface& operator++(int) { release(); return *this; }

        /// release operator
        void operator++() { release(); }

        /// acquire operator, wait forever
        SemaphoreInterface& operator--(int) { acquire(); return *this; }

        /// acquire operator, wait forever
        void operator--() { acquire(); }
    };

    struct SemaphoreAttributes {
        uint32_t maxCount = 1; 
        uint32_t initialCount = 0; 
        const char* name = nullptr;
    };

    /// FreeRTOS static semaphore
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class Semaphore : public SemaphoreInterface {
        StaticSemaphore_t controlBlock = {};

    public:
        /// default constructor
        constexpr Semaphore() : SemaphoreInterface() {}

        Semaphore(const Semaphore&) = delete; ///< disable copy constructor
        Semaphore(Semaphore&& t) = delete;    ///< disable move constructor

        Semaphore& operator=(const Semaphore&) = delete;  ///< disable copy assignment
        Semaphore& operator=(Semaphore&&) = delete;       ///< disable move assignment

        /// initiate semaphore
        /// @param attributes
        ///     - .maxCount maximum token counter, default = 1
        ///     - .initialCount initial token counter, default = 0
        ///     - .name as null terminated string, default = null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(SemaphoreAttributes attributes = {}) {
            if (this->id) return osError;

            osSemaphoreAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            
            this->id = osSemaphoreNew(attributes.maxCount, attributes.initialCount, &attr);
            this->referenceCounterInc();
            
            return osOK;
        }

        /// deinit semaphore
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };    
    
    /// create dynamic semaphore
    /// @param attributes
    ///     - .maxCount maximum token counter, default = 1
    ///     - .initialCount initial token counter, default = 0
    ///     - .name as null terminated string, default = null
    /// @return semaphore object
    /// @note cannot be called from ISR
    inline auto semaphore(SemaphoreAttributes attributes = {}) {
        osSemaphoreAttr_t attr = {};
        attr.name = attributes.name;
        return SemaphoreInterface(osSemaphoreNew(attributes.maxCount, attributes.initialCount, &attr));
    }

    /// return reference to the static semaphore
    inline auto semaphore(Semaphore& sem) { return SemaphoreInterface(sem.get()); }

    /// return reference to semaphore pointer
    inline auto semaphore(osSemaphoreId_t sem) { return SemaphoreInterface(sem); }

    /// return reference to the dynamic semaphore
    inline auto semaphore(SemaphoreInterface& sem) { return SemaphoreInterface(sem.get()); }

    /// return reference to the moved dynamic semaphore
    inline auto semaphore(SemaphoreInterface&& sem) { return SemaphoreInterface(etl::move(sem)); }
}

#endif