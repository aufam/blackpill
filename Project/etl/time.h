#ifndef ETL_TIME_H
#define ETL_TIME_H

#include "FreeRTOS.h"
#include "cmsis_os2.h"

namespace Project::etl {

    struct Time {
        static constexpr auto tickFreq = configTICK_RATE_HZ; ///< os tick frequency

        uint32_t tick; ///< os tick

        /// empty constructor
        constexpr Time() : tick(0) {}
 
        /// default constructor, you can specify the tick value 
        constexpr explicit Time(uint32_t tick) : tick(tick) {}

        constexpr explicit operator bool() const { return tick > 0; }

        /// convert to milli seconds
        [[nodiscard]] constexpr float ms() const { return (float) tick * 1e3f / tickFreq; }

        /// convert to seconds
        [[nodiscard]] constexpr float s() const { return (float) tick / tickFreq; }

        /// convert to minutes
        [[nodiscard]] constexpr float min() const { return (float) tick / tickFreq / 60; }

        /// convert to float, the unit is still os ticks
        constexpr explicit operator float() const { return (float) tick; }
        
        /* arithmetic operators */
        constexpr Time operator-(Time other) const { return Time { tick - other.tick }; }
        constexpr Time operator+(Time other) const { return Time { tick + other.tick }; }

        constexpr float operator/(Time other)  const { return (float) tick / (float) other.tick; }
        constexpr Time operator /(int other)   const { return Time { tick / other }; }
        constexpr Time operator *(float other) const { return Time { uint32_t ((float) tick * other) }; }

        constexpr Time& operator-=(Time other) { tick -= other.tick; return *this; }
        constexpr Time& operator+=(Time other) { tick += other.tick; return *this; }

        /* comparison operators */
        constexpr bool operator==(Time other) const { return tick == other.tick; }
        constexpr bool operator!=(Time other) const { return tick != other.tick; }
        constexpr bool operator>=(Time other) const { return tick >= other.tick; }
        constexpr bool operator<=(Time other) const { return tick <= other.tick; }
        constexpr bool operator >(Time other) const { return tick > other.tick; }
        constexpr bool operator <(Time other) const { return tick < other.tick; }
    };
}

namespace Project::etl::time {

    inline static constexpr auto infinite = Time(osWaitForever);
    inline static constexpr auto immediate = Time(0);

    /// convert from milliseconds to Time 
    inline constexpr Time milliseconds(uint32_t v) { return Time { (v * Time::tickFreq + 999ul) / 1000ul }; }

    /// convert from seconds to Time
    inline constexpr Time seconds(uint32_t v) { return Time { (v * Time::tickFreq) }; }

    /// convert from minutes to Time
    inline constexpr Time minutes(uint32_t v) { return Time { (v * Time::tickFreq * 60) }; }
    
    /// get os tick count
    inline Time now() { return Time { osKernelGetTickCount() }; }

    /// calculate elapsed time since t
    inline Time elapsed(Time t) { return now() - t; }

    /// sleep for specified time interval
    /// @note cannot be called from ISR
    inline void sleep(Time time) { osDelay(time.tick); }

    /// sleep until specified time point
    /// @note cannot be called from ISR
    inline void sleepUntil(Time time) { osDelayUntil(time.tick); }
}

namespace Project::etl::literals {
    constexpr auto operator""ms   (unsigned long long v) { return time::milliseconds(v); }
    constexpr auto operator""s    (unsigned long long v) { return time::seconds(v); }
    constexpr auto operator""min  (unsigned long long v) { return time::minutes(v); }
}

#endif //ETL_TIME_H
