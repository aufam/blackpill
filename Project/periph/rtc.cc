#include "periph/rtc.h"

namespace Project::Periph {

    void RealTimeClock::init() {
        update();
        timer.init(1000, [](void *arg) { auto rtc = (RealTimeClock *) arg; rtc->update(); }, this);
    }

    void RealTimeClock::deinit() {
        timer.deinit();
    }

    void RealTimeClock::update() {
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    }

    int RealTimeClock::setDate(uint8_t week_day, uint8_t date, uint8_t month, uint8_t year) {
        sDate.WeekDay = week_day;
        sDate.Date = date;
        sDate.Month = month;
        sDate.Year = year;
        return HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    }

    int RealTimeClock::setTime(uint8_t secs, uint8_t mins, uint8_t hrs) {
        sTime.Seconds = secs;
        sTime.Minutes = mins;
        sTime.Hours = hrs;
        return HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    }

} // namespace Project