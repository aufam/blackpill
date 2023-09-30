#ifndef ETL_THREAD_H
#define ETL_THREAD_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/array.h"
#include "etl/event.h"
#include "etl/function.h"
#include "etl/time.h"

namespace Project::etl {

    /// FreeRTOS thread interface
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    class ThreadInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");

    protected:
        osThreadId_t id; ///< thread pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticTask_t*>(id)->uxDummy10[1]; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticTask_t*>(id)->uxDummy10[1]; }

    public:
        /// empty constructor
        constexpr ThreadInterface() : id(nullptr) {}

        /// construct from thread pointer
        explicit ThreadInterface(osThreadId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        ThreadInterface(const ThreadInterface& t) : id(t.id) {
            referenceCounterInc();
        }

        /// copy assignment
        ThreadInterface& operator=(const ThreadInterface& t) {
            if (id == t.id) return *this;
            detach();
            id = t.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        ThreadInterface(ThreadInterface&& t) : id(etl::exchange(t.id, nullptr)) {}

        /// move assignment
        ThreadInterface& operator=(ThreadInterface&& t) { 
            if (id == t.id) return *this;
            detach();
            id = etl::exchange(t.id, nullptr); 
            return *this; 
        }

        /// default destructor
        ~ThreadInterface() { detach(); }

        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return id ? reinterpret_cast<StaticTask_t*>(id)->uxDummy10[1] : 0; }

        /// get thread pointer
        osThreadId_t get() { return id; }

        /// terminate execution, delete resource, and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0)
                return osOK;
            return osThreadTerminate(etl::exchange(id, nullptr)); 
        }

        /// name as null terminated string
        /// @note cannot be called from ISR
        const char* getName() { return osThreadGetName(id); }

        /// return current thread state osThreadXxx
        /// @note cannot be called from ISR
        osThreadState_t getState() { return osThreadGetState(id); }

        /// remaining stack size in bytes
        /// @note cannot be called from ISR
        uint32_t getSpace() { return osThreadGetStackSpace(id); }

        /// change the priority
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t setPriority(osPriority_t prio) { return osThreadSetPriority(id, prio); }

        /// return the priority value
        /// @note cannot be called from ISR
        osPriority_t getPriority() { return osThreadGetPriority(id); }

        /// suspend execution of a thread
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t suspend() { return osThreadSuspend(id); }  

        /// resume execution of a thread
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t resume() { return osThreadResume(id); }

        /// set flags to this thread
        /// @param flags specifies the flags of the thread that shall be set
        /// @return this thread's flags after setting or error code if highest bit set
        /// @note can be called from ISR
        FlagManager setFlags(uint32_t flags) { return osThreadFlagsSet(id, flags); }

        /// set flags operator
        ThreadInterface& operator|(uint32_t flags) { setFlags(flags); return *this; }
    };

    struct StaticThreadAttributes {
        etl::Function<void(), void*> function;
        osPriority_t prio = osPriorityNormal; 
        const char* name = nullptr; 
    };
    
    /// FreeRTOS static thread
    /// @tparam N buffer length (in words)
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    /// @note invoke init method before using
    template <size_t N = configMINIMAL_STACK_SIZE>
    class Thread : public ThreadInterface {
        StaticTask_t controlBlock = {};
        Array<uint32_t, N> buffer = {};

    public:
        /// default constructor
        constexpr Thread() : ThreadInterface() {}

        Thread(const Thread&) = delete; ///< disable copy constructor
        Thread(Thread&& t) = delete;    ///< disable move constructor

        Thread& operator=(const Thread&) = delete;  ///< disable copy assignment
        Thread& operator=(Thread&&) = delete;       ///< disable move assignment

        /// initiate thread
        /// @param attributes
        ///     - .function thread function
        ///     - .prio osPriorityXxx, default = osPriorityNormal
        ///     - .name as null terminated string, default = null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(StaticThreadAttributes attributes) { 
            if (this->id) return osError;

            osThreadAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            attr.stack_mem = &buffer;
            attr.stack_size = sizeof(buffer); // in bytes
            attr.priority = attributes.prio;
            
            this->id = osThreadNew(attributes.function.fn, attributes.function.context, &attr);
            this->referenceCounterInc();
            
            return osOK;
        }

        /// detach thread
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    struct DynamicThreadAttributes {
        etl::Function<void(), void*> function;
        uint32_t stackSize = configMINIMAL_STACK_SIZE;
        osPriority_t prio = osPriorityNormal; 
        const char* name = nullptr; 
    };

    /// create dynamic thread
    /// @param attributes
    ///     - .function thread function
    ///     - .stackSize in words (1 word = 4 bytes), default = configMINIMAL_STACK_SIZE
    ///     - .prio osPriorityXxx, default = osPriorityNormal
    ///     - .name as null terminated string, default = null
    /// @return thread object
    /// @note cannot be called from ISR
    inline auto thread(DynamicThreadAttributes attributes) {
        osThreadAttr_t attr = {};
        attr.name = attributes.name;
        attr.priority = attributes.prio;
        attr.stack_size = attributes.stackSize * 4;

        return ThreadInterface(osThreadNew(attributes.function.fn, attributes.function.context, &attr)); 
    }

    /// return reference to the static thread
    template <size_t N>
    auto thread(Thread<N>& thd) { return ThreadInterface(thd.get()); }

    /// return reference to thread pointer
    inline auto thread(osThreadId_t thd) { return ThreadInterface(thd); }

    /// return reference to the dynamic thread
    inline auto thread(ThreadInterface& thd) { return ThreadInterface(thd.get()); }

    /// return reference to the moved dynamic thread
    inline auto thread(ThreadInterface&& thd) { return ThreadInterface(etl::move(thd)); }

    /// return number of active threads
    /// @note cannot be called from ISR
    inline uint32_t threadCount() { return osThreadGetCount(); }

    /// return vector of all threads
    /// @note cannot be called from ISR
    inline auto threadGetAll() {     
        class ThreadArray {
            osThreadId_t* ptr;
            uint32_t n;

        public:
            ThreadArray(const ThreadArray&) = delete;
            ThreadArray& operator=(const ThreadArray&) = delete;

            ThreadArray() : ptr(nullptr), n(threadCount()) {
                ptr = new osThreadId_t[n];
                n = osThreadEnumerate(ptr, n);
            }

            ~ThreadArray() { delete[] ptr; }

            uint32_t len() { return n; }

            ThreadInterface* begin() { return reinterpret_cast<ThreadInterface*>(ptr); }

            ThreadInterface* end() { return reinterpret_cast<ThreadInterface*>(ptr + n); }
            
            ThreadInterface operator[](int i) {
                auto l = len();
                if (l == 0) return ThreadInterface(nullptr);
                if (i < 0) i = l + i; // allowing negative index
                return ThreadInterface(ptr[i]);
            }
        };
        return ThreadArray();
    }
}

namespace Project::etl::this_thread {

    using time::sleep;
    using time::sleepUntil;

    /// return the current running thread object
    /// @note should be called in thread function
    inline auto get() { return ThreadInterface(osThreadGetId()); }

    /// pass control from current thread to next thread
    /// @return osStatus
    /// @note should be called in thread function
    inline osStatus_t yield() { return osThreadYield(); }

    /// terminate execution of current running thread
    /// @note should be called in thread function
    inline void exit() { osThreadExit(); }

    /// reset the specified flags of current running thread
    /// @param flags specifies the flags of the thread that shall be reset
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline FlagManager resetFlags(uint32_t flags) { return osThreadFlagsClear(flags); }

    /// get the current flags of the current running thread
    /// @return current thread's flags
    /// @note should be called in thread function
    inline FlagManager getFlags() { return osThreadFlagsGet(); }

    /// wait for flags of the current running thread to become signaled
    /// @param args
    ///     - .flags specifies the flags to wait for
    ///     - .option osFlagsWaitAny (default) or osFlagsWaitAll
    ///     - .timeout default = osWaitForever
    ///     - .doReset specifies wether reset the flags or not, default = true
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline FlagManager waitFlags(Event::WaitFlagsArgs args) { 
        if (!args.doReset) args.option |= osFlagsNoClear;
        return osThreadFlagsWait(args.flags, args.option, args.timeout.tick); 
    }

    /// wait for any flags of the current running thread to become signaled
    /// @param args
    ///     - .timeout default = osWaitForever
    ///     - .doReset specifies wether reset the flags or not, default = true
    /// @return current thread's flags before resetting or error code if highest bit set
    /// @note should be called in thread function
    inline FlagManager waitFlagsAny(Event::WaitFlagsAnyArgs args = {}) { 
        uint32_t flags = (1u << 24) - 1; // all possible flags
        uint32_t option = osFlagsWaitAny;
        if (!args.doReset) option |= osFlagsNoClear;
        return osThreadFlagsWait(flags, option, args.timeout.tick); 
    }
}

#endif //ETL_THREAD_H
