#ifndef PROJECT_PERIPH_RTC_H
#define PROJECT_PERIPH_RTC_H

#include "../../Core/Inc/rtc.h"
#include "os.h"

namespace Project::Periph {

    /// RTC peripheral class. requirements: no RTC output
    struct RealTimeClock {
        inline static const char *days[7] = {
                "Sun",
                "Mon",
                "Tue",
                "Wed",
                "Thu",
                "Fri",
                "Sat"
        };
        RTC_TimeTypeDef sTime = {};
        RTC_DateTypeDef sDate = {};
        OS::TimerStatic timer;
        constexpr RealTimeClock() = default;

        void update(); ///< get time and date and store to sTime and sDate
        void init(); ///< init timer
        void deinit(); ///< deinit timer
        int setDate(uint8_t week_day, uint8_t date, uint8_t month, uint8_t year);
        int setTime(uint8_t secs, uint8_t mins, uint8_t hrs);

        auto &getSeconds()  { return sTime.Seconds; }
        auto &getMinutes()  { return sTime.Minutes; }
        auto &getHours()    { return sTime.Hours; }
        auto &getDay()  { return days[sDate.WeekDay]; }
        auto &getWeekDay()  { return sDate.WeekDay; }
        auto &getDate()     { return sDate.Date; }
        auto &getMonth()    { return sDate.Month; }
        auto &getYear()     { return sDate.Year; }

    };

    inline RealTimeClock rtc;

} // namespace Project


#endif // PROJECT_PERIPH_RTC_H