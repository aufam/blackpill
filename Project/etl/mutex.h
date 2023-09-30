#ifndef ETL_MUTEX_H
#define ETL_MUTEX_H

#include "etl/thread.h"

namespace Project::etl {

    /// FreeRTOS mutex interface.
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    class MutexInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");

    protected:
        osMutexId_t id; ///< mutex pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }

    public:
        /// empty constructor
        constexpr MutexInterface() : id(nullptr) {}

        /// construct from mutex
        explicit MutexInterface(osMutexId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        MutexInterface(const MutexInterface& m) : id(m.id) {
            referenceCounterInc();
        }

        /// copy assignment
        MutexInterface& operator=(const MutexInterface& m) {
            if (id == m.id) return *this;
            detach();
            id = m.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        MutexInterface(MutexInterface&& m) : id(etl::exchange(m.id, nullptr)) {}

        /// move assignment
        MutexInterface& operator=(MutexInterface&& m) { 
            if (id == m.id) return *this;
            detach(); 
            id = etl::exchange(m.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~MutexInterface() { detach(); }
        
        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return reinterpret_cast<StaticSemaphore_t*>(id)->uxDummy8; }

        osMutexId_t get() { return id; }

        /// unlock mutex, delete resource, and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0)
                return osOK;
            
            unlock(); 
            return osMutexDelete(etl::exchange(id, nullptr)); 
        }

        /// name as null terminated string
        const char* getName() { return osMutexGetName(id); }  

        /// lock mutex
        /// @param timeout default = timeInfinite
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t lock(etl::Time timeout = etl::time::infinite) { return osMutexAcquire(id, timeout.tick); }

        /// unlock mutex
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t unlock() { return osMutexRelease(id); }

        /// get thread that owns this mutex
        /// @return thread object
        /// @note cannot be called from ISR
        auto getOwner() { return etl::ThreadInterface(osMutexGetOwner(id)); }
    };

    struct MutexAttributes {
        const char* name = nullptr; 
    };

    /// FreeRTOS static mutex.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    class Mutex : public MutexInterface {
        StaticSemaphore_t controlBlock = {};

    public:
        /// default constructor
        constexpr Mutex() : MutexInterface() {}

        Mutex(const Mutex&) = delete; ///< disable copy constructor
        Mutex(Mutex&& t) = delete;    ///< disable move constructor

        Mutex& operator=(const Mutex&) = delete;  ///< disable copy assignment
        Mutex& operator=(Mutex&&) = delete;       ///< disable move assignment

        /// initiate mutex
        /// @param name string name, default null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(MutexAttributes attributes = {}) {
            if (this->id) return osError;
            
            osMutexAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);

            this->id = osMutexNew(&attr);
            this->referenceCounterInc();
            return osOK;
        }

        /// deinit mutex
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic mutex
    inline auto mutex(MutexAttributes attributes = {}) { 
        osMutexAttr_t attr = {};
        attr.name = attributes.name;
        return MutexInterface(osMutexNew(&attr)); 
    }

    /// return reference to the static mutex
    inline auto mutex(Mutex& mtx) { return MutexInterface(mtx.get()); }

    /// return reference to mutex pointer
    inline auto mutex(osMutexId_t mtx) { return MutexInterface(mtx); }

    /// return reference to the dynamic mutex
    inline auto mutex(MutexInterface& mtx) { return MutexInterface(mtx.get()); }

    /// return reference to the moved dynamic mutex
    inline auto mutex(MutexInterface&& mtx) { return MutexInterface(etl::move(mtx)); }

    /// lock mutex when entering a scope and unlock when exiting
    struct MutexScope {
        osMutexId_t id;

        explicit MutexScope(osMutexId_t mutex, etl::Time timeout = etl::time::infinite) : id(mutex) {
            osMutexAcquire(id, timeout.tick);
        }

        ~MutexScope() { osMutexRelease(id); }
    };

    /// lock scope
    /// @return mutex scope object
    /// @note cannot be called from ISR
    [[nodiscard]] 
    inline auto lockScope(MutexInterface& mutex) { return MutexScope(mutex.get()); }

    /// lock scope
    /// @return mutex scope object
    /// @note cannot be called from ISR
    [[nodiscard]] 
    inline auto lockScope(Mutex& mutex) { return MutexScope(mutex.get()); }

    /// lock scope
    /// @return mutex scope object
    /// @note cannot be called from ISR
    [[nodiscard]] 
    inline auto lockScope(osMutexId_t mutex) { return MutexScope(mutex); }
}

#endif //ETL_MUTEX_H
