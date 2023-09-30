#ifndef ETL_NUMERICS_H
#define ETL_NUMERICS_H

#include "etl/optional.h"
#include <cmath> // std::floor and std::trunc

namespace Project::etl::detail {
    
    /// This type trait will return long long if T and U are integrals but of different signedness; otherwise, it will return their common type
    template <typename T, typename U> using safe_arithmetic_operation_return_t = conditional_t
        <is_unsigned_integral_v<T> ^ is_unsigned_integral_v<U>, conditional_t
            <is_floating_point_v<T> ^ is_floating_point_v<U>, 
                common_type_t<T, U>,
                long long
            >, 
            common_type_t<T, U>
        >;

    template <typename T>
    struct DivMod { T quo, rem; };
}

namespace Project::etl {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

    /// type safe less than
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_lt(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? true : x < y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? false : x < y;
        else
            return x < y;
    }

    /// type safe less equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_le(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? true : x <= y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? false : x <= y;
        else
            return x < y;
    }

    /// type safe greater than
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_gt(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? false : x > y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? true : x > y;
        else
            return x > y;
    }

    /// type safe greater equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_ge(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? false : x >= y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? true : x >= y;
        else
            return x >= y;
    }

    /// type safe equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_eq(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? false : x == y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? false : x == y;
        else
            return x == y;
    }

    /// type safe not equal
    template <typename T, typename U> enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, bool> 
    safe_ne(T x, U y) {
        if constexpr (is_signed_integral_v<T> && is_unsigned_integral_v<U>)
            return x < T(0) ? true : x != y;
        if constexpr (is_unsigned_integral_v<T> && is_signed_integral_v<U>)
            return y < U(0) ? true : x != y;
        else
            return x != y;
    }

#pragma GCC diagnostic pop

    /// type safe numeric casting
    template <typename R, typename T> enable_if_t<is_arithmetic_v<R> && is_arithmetic_v<T>, R>
    safe_cast(T x) {
        if constexpr (is_integral_v<R> && is_floating_point_v<T>)
            return static_cast<R>(std::round(x));
        else
            return static_cast<R>(x);
    }

    /// type safe numeric casting
    template <typename R, typename T> enable_if_t<is_arithmetic_v<R> && is_arithmetic_v<T>, R>
    safe_cast_floor(T x) {
        if constexpr (is_integral_v<R> && is_floating_point_v<T>)
            return static_cast<R>(std::floor(x));
        else
            return static_cast<R>(x);
    }

    /// type safe numeric casting
    template <typename R, typename T> enable_if_t<is_arithmetic_v<R> && is_arithmetic_v<T>, R>
    safe_cast_trunc(T x) {
        if constexpr (is_integral_v<R> && is_floating_point_v<T>)
            return static_cast<R>(std::trunc(x));
        else
            return static_cast<R>(x);
    }

    /// type safe addition
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, RR>
    safe_add(T x, U y) { return etl::safe_cast<RR>(etl::safe_cast<R>(x) + etl::safe_cast<R>(y)); }

    /// type safe substraction
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, RR>
    safe_sub(T x, U y) { return etl::safe_cast<RR>(etl::safe_cast<R>(x) - etl::safe_cast<R>(y)); }
    
    /// type safe multiplication
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, RR>
    safe_mul(T x, U y) { return etl::safe_cast<RR>(etl::safe_cast<R>(x) * etl::safe_cast<R>(y)); }

    /// type safe division
    /// @note if y is integral of 0, return 0  
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, RR>
    safe_div(T x, U y) { 
        if constexpr (is_integral_v<RR> && is_integral_v<U>)
            return y == U(0) ? RR(0) : etl::safe_cast<RR>(etl::safe_cast<R>(x) / etl::safe_cast<R>(y));
        else
            return etl::safe_cast<RR>(etl::safe_cast<common_type_t<R, RR>>(x) / etl::safe_cast<common_type_t<R, RR>>(y));
    }    
    
    /// type safe floor division, calculate the floor of x / y 
    /// @note if y is integral of 0, return 0  
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, RR>
    safe_floordiv(T x, U y) { 
        if constexpr (is_integral_v<RR> && is_integral_v<U>)
            return y == U(0) ? RR(0) : etl::safe_cast_floor<RR>(etl::safe_cast<R>(x) / etl::safe_cast<R>(y));
        else
            return etl::safe_cast_floor<RR>(etl::safe_cast<common_type_t<R, RR>>(x) / etl::safe_cast<common_type_t<R, RR>>(y));
    }

    /// type safe true division
    /// @note return type has to be floating point
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, conditional_t<is_floating_point_v<R>, R, long double>, ReturnType>>
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U> && is_floating_point_v<RR>, RR>
    safe_truediv(T x, U y) { return etl::safe_div<RR>(x, y); }

    /// type safe modulo
    /// @note if y is integral of 0, return x
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, RR>
    safe_mod(T x, U y) {
        if constexpr (is_integral_v<RR> && (is_floating_point_v<T> || is_floating_point_v<U>)) {
            auto quo = std::trunc(etl::safe_truediv(x, y));
            auto rem = etl::safe_sub(x, etl::safe_mul(quo, y));
            bool valid = !(std::isnan(quo) || std::isinf(quo));
            return valid ? etl::safe_cast<RR>(rem) : etl::safe_cast<RR>(x);
        }
        else if constexpr (is_integral_v<RR>) {
            if (y == U(0)) return etl::safe_cast<RR>(x);
            auto remainder = etl::safe_cast<RR>(x) % etl::safe_cast<RR>(y);
            return etl::safe_cast<RR>(remainder);
        } 
        else {
            auto quo = std::trunc(etl::safe_truediv(x, y));
            auto rem = etl::safe_sub(x, etl::safe_mul(quo, y));
            return rem;
        }
    }

    /// type safe division and modulo
    /// @note if y is integral of 0, return (0, x) 
    template <typename ReturnType = void, typename T, typename U,
              typename R = detail::safe_arithmetic_operation_return_t<T, U>,
              typename RR = conditional_t<is_void_v<ReturnType>, R, ReturnType>> 
    enable_if_t<is_arithmetic_v<T> && is_arithmetic_v<U>, detail::DivMod<RR>>
    safe_divmod(T x, U y) { 
        if constexpr (is_integral_v<RR> && (is_floating_point_v<T> || is_floating_point_v<U>)) {
            detail::DivMod<RR> res {};
            auto quo = std::trunc(etl::safe_truediv(x, y));
            bool valid = !(std::isnan(quo) || std::isinf(quo));
            res.quo = valid ? etl::safe_cast<RR>(quo) : RR(0);
            res.rem = valid ? etl::safe_sub<RR>(x, etl::safe_mul(quo, y)) : etl::safe_cast<RR>(x);
            if (res.rem < 0) res.rem = -res.rem;
            return res;
        }
        if constexpr (is_integral_v<RR>) {
            detail::DivMod<RR> res {};
            res.quo = static_cast<RR>(0);
            res.rem = static_cast<RR>(x);
            if (y == U(0)) return res;

            res.quo = static_cast<RR>(x) / static_cast<RR>(y);
            res.rem = static_cast<RR>(x) % static_cast<RR>(y);
            if (res.rem < 0) res.rem = -res.rem;
            return res;
        } 
        else {
            detail::DivMod<RR> res {};
            res.quo = std::trunc(etl::safe_truediv(x, y));
            res.rem = etl::safe_sub(x, etl::safe_mul(res.quo, y));
            if (res.rem < 0 && !std::isnan(res.rem)) res.rem = -res.rem;
            return res;
        }
    }

}

#endif