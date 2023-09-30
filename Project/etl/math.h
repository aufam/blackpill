#ifndef ETL_MATH_H
#define ETL_MATH_H

#include "etl/algorithm.h"
#include <cmath>

namespace Project::etl::literals {
    constexpr size_t operator ""_k(unsigned long long val)  { return val * 1000; }
    constexpr size_t operator ""_k(long double val)         { return size_t(val * 1000); }

    constexpr size_t operator ""_M(unsigned long long val)  { return val * 1000_k; }
    constexpr size_t operator ""_M(long double val)         { return size_t(val * 1000_k); }

    constexpr size_t operator ""_G(unsigned long long val)  { return val * 1000_M; }
    constexpr size_t operator ""_G(long double val)         { return size_t(val * 1000_M); }

    constexpr float operator ""_pi(long double val)                 { return float(val) * float(M_PI); }
    constexpr float operator ""_pi(unsigned long long val)          { return float(val) * float(M_PI); }
    constexpr float operator ""_pi_half(long double val)            { return float(val) * float(M_PI_2); }
    constexpr float operator ""_pi_half(unsigned long long val)     { return float(val) * float(M_PI_2); }
    constexpr float operator ""_pi_quarter(long double val)         { return float(val) * float(M_PI_4); }
    constexpr float operator ""_pi_quarter(unsigned long long val)  { return float(val) * float(M_PI_4); }
    constexpr float operator ""_pi_inverse(long double val)         { return float(val) * float(M_1_PI); }
    constexpr float operator ""_pi_inverse(unsigned long long val)  { return float(val) * float(M_1_PI); }

    constexpr float operator ""_square_root_of_2(long double val)                 { return float(val) * float(M_SQRT2); }
    constexpr float operator ""_square_root_of_2(unsigned long long val)          { return float(val) * float(M_SQRT2); }
    constexpr float operator ""_inverse_square_root_of_2(long double val)         { return float(val) * float(M_SQRT1_2); }
    constexpr float operator ""_inverse_square_root_of_2(unsigned long long val)  { return float(val) * float(M_SQRT1_2); }

    constexpr float operator ""_e(long double val)                 { return float(val) * float(M_E); }
    constexpr float operator ""_e(unsigned long long val)          { return float(val) * float(M_E); }
    constexpr float operator ""_log2e(long double val)             { return float(val) * float(M_LOG2E); }
    constexpr float operator ""_log2e(unsigned long long val)      { return float(val) * float(M_LOG2E); }
    constexpr float operator ""_log10e(long double val)            { return float(val) * float(M_LOG10E); }
    constexpr float operator ""_log10e(unsigned long long val)     { return float(val) * float(M_LOG10E); }
    constexpr float operator ""_ln2(long double val)               { return float(val) * float(M_LN2); }
    constexpr float operator ""_ln2(unsigned long long val)        { return float(val) * float(M_LN2); }
    constexpr float operator ""_ln10(long double val)              { return float(val) * float(M_LN10); }
    constexpr float operator ""_ln10(unsigned long long val)       { return float(val) * float(M_LN10); }
}

namespace Project::etl {

    /// computes absolute value
    template <typename T> constexpr enable_if_t<is_signed_v<T>, T>
    absolute(T value) { return value < T(0) ? -value : value; }
    
    template <typename T> constexpr enable_if_t<is_unsigned_v<T>, T>
    absolute(T value) { return value; }

    // return the sign of the argument. -1 if negative, 1 if zero or positive.
    template <typename X> constexpr int
    sign(X x) { return x < X(0) ? -1 : 1; }

    template <typename X> constexpr auto
    square(X x) { return x * x; }

    template <typename X> constexpr auto
    sqrt(X x) { return std::sqrt(x); }

    template <typename X> constexpr auto
    is_infinite(X x) { return std::isinf(x); }

    template <typename X> constexpr auto
    is_nan(X x) { return std::isnan(x); }

    template <typename T, typename U> constexpr enable_if_t<is_floating_point_v<U>, T>
    low_pass_fast(T value, T next, U constant) { return value - ((value - next) * constant); }

    template <typename T> constexpr T
    moving_avg_fast(T value, T next, size_t N) { 
        return etl::low_pass_fast(value, next, 2.f / (static_cast<float>(N) + 1.f)); 
    }    
    
    /// interpolate x given [x1, y1] and [x2, y2]
    /// @param trim clamp the result to range (y1, y2). default = true
    template <class X, class Y> constexpr Y
    interpolate(const X& x, const X& x1, const X& x2, const Y& y1, const Y& y2, bool trim = true) {
        Y res = y1 + (Y) (static_cast<float>(y2 - y1) * static_cast<float>(x - x1) / static_cast<float>(x2 - x1));
        return trim ? etl::clamp(res, y1, y2) : res;
    }

    /// overload
    template <class X, class Y> constexpr Y
    interpolate(const X& x, const Pair<X,Y>& p1, const Pair<X,Y>& p2, bool trim = true) {
        auto& [x1, y1] = p1;
        auto& [x2, y2] = p2;
        return etl::interpolate(x, x1, x2, y1, y2, trim);
    }

    /// Phase angle struct
    struct Phase {
        static constexpr float pi2_ = 2.f * M_PI;
        /// fold angle to range[-pi, pi]
        static constexpr float
        fold(float radian) { return radian - std::floor(radian / pi2_ + .5f) * pi2_; }

        /// convert angle to fixed value
        static constexpr int32_t
        toFix32(float radian) { return static_cast<int32_t> (fold(radian) * static_cast<float> (0xFFFFFFFF) / pi2_); }
        static constexpr int16_t
        toFix16(float radian) { return static_cast<int16_t> (fold(radian) * static_cast<float> (0xFFFF) / pi2_); }
        static constexpr int8_t
        toFix8(float radian) { return static_cast<int8_t> (fold(radian) * static_cast<float> (0xFF) / pi2_); }

        /// convert fixed value to angle in radian
        static constexpr float
        fromFix(int32_t angle) { return static_cast<float> (angle) * pi2_ / static_cast<float> (0xFFFFFFFF); }
        static constexpr float
        fromFix(int16_t angle) { return static_cast<float> (angle) * pi2_ / static_cast<float> (0xFFFF); }
        static constexpr float
        fromFix(int8_t angle) { return static_cast<float> (angle) * pi2_ / static_cast<float> (0xFF); }
    };
}

#endif //ETL_MATH_H
