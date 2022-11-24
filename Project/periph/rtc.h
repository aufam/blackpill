#ifndef PROJECT_PERIPH_RTC_H
#define PROJECT_PERIPH_RTC_H

#include "../../Core/Inc/rtc.h"
#include "etl/timer.h"

namespace Project::Periph {

    /// RTC peripheral class
    /// @note requirements: no RTC output
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
        etl::Timer timer;
        constexpr RealTimeClock() = default;

        /// get time and date and store to sTime and sDate
        void update() {
            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        }

        /// update and init timer
        void init() {
            update();
            timer.init(1000,
                       [](void *arg) { ((RealTimeClock *) arg)->update(); },
                       this);
        }

        /// deinit timer
        void deinit() { timer.deinit(); }

        int setDate(uint8_t week_day, uint8_t date, uint8_t month, uint8_t year) {
            sDate.WeekDay = week_day;
            sDate.Date = date;
            sDate.Month = month;
            sDate.Year = year;
            return HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
        }

        int setTime(uint8_t secs, uint8_t mins, uint8_t hrs) {
            sTime.Seconds = secs;
            sTime.Minutes = mins;
            sTime.Hours = hrs;
            return HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        }

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