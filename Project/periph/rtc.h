#ifndef PERIPH_RTC_H
#define PERIPH_RTC_H

#include "Core/Inc/rtc.h"
#include "etl/time.h"

namespace Project::periph {

    /// RTC peripheral class
    /// @note requirements: no RTC output
    class RealTimeClock {
        inline static const char days[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
        inline static const auto minimumUpdateInterval = etl::time::milliseconds(500);
        inline static auto lastUpdate = etl::Time(0);
    
    public:
        RTC_TimeTypeDef sTime = {};
        RTC_DateTypeDef sDate = {};

        /// default constructor
        constexpr RealTimeClock() = default;

        RealTimeClock(const RealTimeClock&) = delete; ///< disable copy constructor
        RealTimeClock& operator=(const RealTimeClock&) = delete;  ///< disable copy assignment

        /// get time and date and store to sTime and sDate
        void update() {
            auto now = etl::time::now();
            if (now - lastUpdate < minimumUpdateInterval) return;

            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
            lastUpdate = now;
        }

        int setDate(uint8_t week_day, uint8_t date, uint8_t month, uint8_t year) {
            sDate.WeekDay = week_day;
            sDate.Date = date;
            sDate.Month = month;
            sDate.Year = year;
            return HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        }

        int setTime(uint8_t hrs, uint8_t mins, uint8_t secs) {
            sTime.Hours = hrs;
            sTime.Minutes = mins;
            sTime.Seconds = secs;
            return HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        }

        auto getSeconds()  { update(); return sTime.Seconds; }
        auto getMinutes()  { update(); return sTime.Minutes; }
        auto getHours()    { update(); return sTime.Hours; }
        auto getDay()      { update(); return days[sDate.WeekDay]; }
        auto getWeekDay()  { update(); return sDate.WeekDay; }
        auto getDate()     { update(); return sDate.Date; }
        auto getMonth()    { update(); return sDate.Month; }
        auto getYear()     { update(); return sDate.Year; }
    };

    inline RealTimeClock rtc;

} // namespace Project


#endif // PERIPH_RTC_H