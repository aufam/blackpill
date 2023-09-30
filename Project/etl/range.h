#ifndef ETL_RANGE_H
#define ETL_RANGE_H

#include "etl/iter.h"

namespace Project::etl {

    /// python-like range
    template <typename T>
    class Range {
        typedef remove_unsigned_t<T> U;
        T start, stop;
        U step;

    public:
//        friend class RangeHelper;

        constexpr Range(T start, T stop, U step) : start(start), stop(stop), step(step) {}

        constexpr Range begin() const { return *this; }
        constexpr Range end()   const { return *this; }

        constexpr Range iter()      const { return *this; }
        constexpr Range reversed()  const { return Range(stop - step, start - step, -step); }

        constexpr size_t len() const { return operator bool() ? (stop - start) / step : 0; }

        constexpr T operator[](int index) const {
            auto n = int(len());
            if (index < 0) index += n;
            if (index < 0 || index >= n) return T(0);
            return start + T(index) * step;
        }

        /// slice operator
        constexpr Range operator()(int start, int stop, int step = 1) const {
            auto n = int(len());
            if (start < 0) start += n;
            if (start < 0 || start >= n) return Range(this->start, this->start, U(0));
            auto first = this->start + T(start) * this->step;

            if (stop < 0) stop += n;
            if (stop < 0 || stop >= n) return Range(first, first, U(0));
            auto last = this->start + T(stop) * this->step;

            return Range(first, last, step * this->step);
        }

        constexpr explicit operator bool() const {
            return step == U(0) ? false : step < U(0) ? start > stop : start < stop;
        }

        constexpr bool operator!=(const Range&) const { return operator bool(); }

        /// increment operator
        constexpr void operator++() { start += step; }

        /// dereference operator
        constexpr T operator*() { return start; }

        /// next operator
        constexpr T operator()() { 
            auto valid = operator bool();
            auto res = valid ? start : T();
            if (valid) operator++();
            return res;
        }
    };

    /// create range object
    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T last) { return Range(T(0), last, remove_unsigned_t<T>(1)); }

    /// create range object
    template <typename T> constexpr enable_if_t<is_arithmetic_v<T>, Range<T>>
    range(T first, T last, remove_unsigned_t<T> step = 1) { return Range(first, last, step); }
}

#endif // ETL_RANGE_H
