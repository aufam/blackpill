#ifndef ETL_EVENT_H
#define ETL_EVENT_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "etl/utility.h"
#include "etl/time.h"

namespace Project::etl {

    // provides a convenient way to manage and manipulate flags represented by a 32-bit unsigned integer
    class FlagManager {
        uint32_t flags;
    
    public:
        // construct a FlagManager object with the specified initial set of flags
        FlagManager(uint32_t flags) : flags(flags) {}

        // return true if osFlagsError flag is not set
        explicit operator bool() const { return !(flags & osFlagsError); }

        // return true if osFlagsError flag is not set and the specified flags are set
        bool operator&(uint32_t other) const { return operator bool() && (flags & other); }

        // return true if osFlagsError flag is not set and the specified flags are the same as this flags
        bool operator==(uint32_t other) const { return operator bool() && (flags == other); }

        bool operator!=(uint32_t other) const { return !operator ==(other); }

        // return raw flags
        uint32_t get() const { return flags; }
    };

    /// FreeRTOS event interface.
    /// @note requires cmsis os v2, USE_TRACE_FACILITY, SUPPORT_STATIC_ALLOCATION, SUPPORT_DYNAMIC_ALLOCATION
    /// @note should not be declared as const
    class EventInterface {
        static_assert(configUSE_TRACE_FACILITY > 0, "configUSE_TRACE_FACILITY has to be activated");
        static_assert(configSUPPORT_STATIC_ALLOCATION > 0, "configSUPPORT_STATIC_ALLOCATION has to be activated");
        static_assert(configSUPPORT_DYNAMIC_ALLOCATION > 0, "configSUPPORT_DYNAMIC_ALLOCATION has to be activated");

    protected:
        osEventFlagsId_t id; ///< event pointer

        void referenceCounterInc() { if (id) ++reinterpret_cast<StaticEventGroup_t*>(id)->uxDummy3; }
        void referenceCounterDec() { if (id) --reinterpret_cast<StaticEventGroup_t*>(id)->uxDummy3; }
        
    public:
        /// empty constructor
        constexpr EventInterface() : id(nullptr) {}

        /// construct from event pointer
        explicit EventInterface(osEventFlagsId_t id) : id(id) {
            referenceCounterInc();
        }

        /// copy constructor
        EventInterface(const EventInterface& e) : id(e.id) {
            referenceCounterInc();
        }

        /// copy assignment
        EventInterface& operator=(const EventInterface& e) {
            if (id == e.id) return *this;
            detach();
            id = e.id;
            referenceCounterInc();
            return *this;
        }

        /// move constructor
        EventInterface(EventInterface&& e) noexcept : id(etl::exchange(e.id, nullptr)) {}

        /// move assignment
        EventInterface& operator=(EventInterface&& e) noexcept {
            if (id == e.id) return *this;
            detach(); 
            id = etl::exchange(e.id, nullptr); 
            return *this;
        }

        /// default destructor
        ~EventInterface() { detach(); }
        
        /// return true if id is not null
        explicit operator bool() { return count() > 0; }

        /// get the reference counter
        uint32_t count() { return reinterpret_cast<StaticEventGroup_t*>(id)->uxDummy3; }

        /// get event pointer
        osEventFlagsId_t get() { return id; }

        /// decrease reference counter and clean resource if reference counter is 0 
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t detach() { 
            referenceCounterDec();
            if (count() > 0) 
                return osOK;
            
            return osEventFlagsDelete(etl::exchange(id, nullptr)); 
        }

        /// name as null terminated string
        const char* getName() { return osEventFlagsGetName(id); }  

        /// set flags of this event
        /// @param flags specifies the flags that shall be set
        /// @return flags after setting or error code if highest bit set
        /// @note can be called from ISR
        FlagManager setFlags(uint32_t flags) { return osEventFlagsSet(id, flags); }

        /// reset the specified flags of this event
        /// @param flags specifies the flags that shall be reset
        /// @return flags before resetting or error code if highest bit set
        /// @note can be called from ISR
        FlagManager resetFlags(uint32_t flags) { return osEventFlagsClear(id, flags); }

        /// get the flags of this event
        /// @return current event's flags
        /// @note can be called from ISR
        FlagManager getFlags() { return osEventFlagsGet(id); }

        struct WaitFlagsArgs {
            uint32_t flags;
            uint32_t option = osFlagsWaitAny;
            Time timeout = time::infinite;
            bool doReset = true;
        };

        /// wait for flags of this event to become signaled
        /// @param args
        ///     - .flags specifies the flags to wait for
        ///     - .option osFlagsWaitAny (default) or osFlagsWaitAll
        ///     - .timeout default = time::infinite
        ///     - .doReset specifies wether reset the flags or not, default = true
        /// @return flags before resetting or error code if highest bit set
        /// @note can be called from ISR if timeout == time::immediate
        FlagManager waitFlags(WaitFlagsArgs args) { 
            if (!args.doReset) args.option |= osFlagsNoClear;
            return osEventFlagsWait(id, args.flags, args.option, args.timeout.tick); 
        }

        struct WaitFlagsAnyArgs {
            Time timeout = time::infinite;
            bool doReset = true;
        };

        /// wait for any flags of this event to become signaled
        /// @param args
        ///     - .timeout default = time::infinite
        ///     - .doReset specifies wether reset the flags or not, default = true
        /// @return flags before resetting or error code if highest bit set
        /// @note can be called from ISR if timeout == 0
        FlagManager waitFlagsAny(WaitFlagsAnyArgs args = {.timeout=time::infinite, .doReset=true}) { 
            uint32_t flags = (1u << 24) - 1; // all possible flags
            uint32_t option = osFlagsWaitAny;
            if (!args.doReset) option |= osFlagsNoClear;
            return osEventFlagsWait(id, flags, option, args.timeout.tick); 
        }

        /// set operator
        EventInterface& operator|(uint32_t flags) { setFlags(flags); return *this; }

        /// reset operator
        EventInterface& operator&(uint32_t flags) { resetFlags(flags); return *this; }
    };

    struct EventAttributes {
        const char* name = nullptr;
    };

    /// FreeRTOS static event.
    /// @note requires cmsis os v2
    /// @note should not be declared as const
    /// @note invoke init method before using
    class Event : public EventInterface {
        StaticEventGroup_t controlBlock = {};

    public:
        /// default constructor
        constexpr Event() : EventInterface() {}

        Event(const Event&) = delete; ///< disable copy constructor
        Event(Event&& t) = delete;    ///< disable move constructor

        Event& operator=(const Event&) = delete;  ///< disable copy assignment
        Event& operator=(Event&&) = delete;       ///< disable move assignment

        /// initiate event
        /// @param name string name, default null
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t init(EventAttributes attributes = {}) {
            if (this->id) return osError;

            osEventFlagsAttr_t attr = {};
            attr.name = attributes.name;
            attr.cb_mem = &controlBlock;
            attr.cb_size = sizeof(controlBlock);
            
            this->id = osEventFlagsNew(&attr);
            this->referenceCounterInc();
            return osOK;
        }

        /// deinit event
        /// @return osStatus
        /// @note cannot be called from ISR
        osStatus_t deinit() { return detach(); }
    };

    /// create dynamic event
    /// @param name as null terminated string, default = null
    /// @return event object
    /// @note cannot be called from ISR
    inline auto event(EventAttributes attributes = {}) {
        osEventFlagsAttr_t attr = {};
        attr.name = attributes.name;
        return EventInterface(osEventFlagsNew(&attr));
    }

    /// return reference to the static event
    inline auto event(Event& evt) { return EventInterface(evt.get()); }

    /// return reference to event pointer
    inline auto event(osEventFlagsId_t evt) { return EventInterface(evt); }

    /// return reference to the dynamic event
    inline auto event(EventInterface& evt) { return EventInterface(evt.get()); }

    /// return reference to the moved dynamic event
    inline auto event(EventInterface&& evt) { return EventInterface(etl::move(evt)); }
}

#endif