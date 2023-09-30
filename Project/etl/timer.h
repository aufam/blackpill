#ifndef ETL_TIMER_H
#define ETL_TIMER_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/function.h"
#include "etl/utility.h"
#include "etl/time.h"

namespace Project::etl {

    /// FreeRTOS timer interface
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    class TimerInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");
        
    protected:
        osTimerId_t id; ///< timer pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticTimer_t*>(id)->uxDummy7; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticTimer_t*>(id)->uxDummy7; }

    public:
        /// empty constructor
        constexpr TimerInterface() : id(nullptr) {}

        /// construct from timer pointer
        explicit TimerInterface(osTimerId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        TimerInterface(const TimerInterface& t) : id(t.id) {
            referenceCounterInc();
        }

        /// copy assignment
        TimerInterface& operator=(const TimerInterface& t) {
            if (id == t.id) return *this;
            detach();
            id = t.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        TimerInterface(TimerInterface&& t) : id(etl::exchange(t.id, nullptr)) {}

        /// move assignment
        TimerInterface& operator=(TimerInterface&& t) { 
            if (id == t.id) return *this;
            detach(); 
            id = etl::exchange(t.id, nullptr); 
            return *this; 
        }

        /// default destructor
        ~TimerInterface() { detach(); }
        
        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return id ? reinterpret_cast<StaticTimer_t*>(id)->uxDummy7 : 0; }

        /// get timer pointer
        osTimerId_t get() { return id; }

        /// stop this timer, detach the callback function and set id to null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0) 
                return osOK;
            
            stop(); 
            return osTimerDelete(etl::exchange(id, nullptr)); 
        }

        /// name as null terminated string
        /// @note cannot be called from ISR
        const char* getName() { return osTimerGetName(id); }

        /// start or restart the timer
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t start(uint32_t interval) { return osTimerStart(id, interval); }

        /// start or restart the timer
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t stop() { return osTimerStop(id); }

        /// check if the timer is running
        /// @note cannot be called from ISR
        bool isRunning() { return osTimerIsRunning(id); }
    };

    struct TimerAttributes {
        etl::Function<void(), void*> function;
        etl::Time interval; 
        osTimerType_t type = osTimerPeriodic; 
        const char *name = nullptr;
        bool startNow = true;
    };

    /// FreeRTOS timer
    /// @note requires cmsis os v2
    class Timer : public TimerInterface {
        StaticTimer_t controlBlock = {};

    public:
        /// empty constructor
        constexpr Timer() : TimerInterface() {}

        Timer(const Timer&) = delete; ///< disable copy constructor
        Timer(Timer&& t) = delete;    ///< disable move constructor

        Timer& operator=(const Timer&) = delete; ///< disable copy assignment
        Timer& operator=(Timer&&) = delete;      ///< disable move assignment

        /// initiate timer
        /// @param fn function
        /// @param ctx function context
        /// @param attributes
        ///     - .function thread function
        ///     - .interval: function call interval.
        ///     - .type: timer type, osTimerPeriodic (default) or osTimerOnce
        ///     - .name: as null terminated string, default = null
        ///     - .startNow: true (default): start now, false: start later
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(TimerAttributes attributes) {
            if (this->id) return osError;

            osTimerAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            
            this->id = osTimerNew(attributes.function.fn, attributes.type, attributes.function.context, &attr);
            this->referenceCounterInc();
            if (attributes.startNow) 
                this->start(attributes.interval.tick);
            
            return osOK;
        }

        /// detach timer
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic timer
    /// @param attributes
    ///     - .function thread function
    ///     - .interval: function call interval.
    ///     - .type: timer type, osTimerPeriodic (default) or osTimerOnce
    ///     - .name: as null terminated string, default = null
    ///     - .startNow: true (default): start now, false: start later
    /// @return timer object
    /// @note cannot be called from ISR
    inline auto timer(TimerAttributes attributes) {
        osTimerAttr_t attr = {};
        attr.name = attributes.name;
        
        auto res = TimerInterface(osTimerNew(attributes.function.fn, attributes.type, attributes.function.context, &attr));
        if (attributes.startNow) res.start(attributes.interval.tick);

        return etl::move(res); 
    }

    /// return reference to the static timer
    inline auto timer(Timer& tim) { return TimerInterface(tim.get()); }

    /// return reference to timer pointer
    inline auto timer(osTimerId_t tim) { return TimerInterface(tim); }

    /// return reference to the dynamic timer
    inline auto timer(TimerInterface& tim) { return TimerInterface(tim.get()); }

    /// return reference to the moved dynamic timer
    inline auto timer(TimerInterface&& tim) { return TimerInterface(etl::move(tim)); }

}

#endif //ETL_TIMER_H
