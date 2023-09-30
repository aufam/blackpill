#ifndef ETL_LOGIC_H
#define ETL_LOGIC_H

#include "etl/utility_basic.h"

namespace Project::etl {

    struct Tribool {
        enum Value { FALSE_, TRUE_, INDETERMINATE_ } value;

        /// empty constructor
        constexpr Tribool() : value(INDETERMINATE_) {}

        /// construct from boolean
        constexpr Tribool(bool value) : value(value ? TRUE_ : FALSE_) {}

        /// disable none type conversion
        Tribool(None) = delete;

        /// cast to boolean, indeterminate value will return false
        constexpr explicit operator bool() const noexcept { return value == TRUE_; }
    };

    inline static constexpr auto False = Tribool(false);
    inline static constexpr auto True = Tribool(true);
    inline static constexpr auto Indeterminate = Tribool();

    /// check if value is indeterminate
    constexpr inline bool 
    is_indeterminate(Tribool x) { return x.value == Tribool::INDETERMINATE_; }

    /// inverse operator
    constexpr inline Tribool 
    operator!(Tribool x) noexcept { return x.value == Tribool::FALSE_ ? True : x.value == Tribool::TRUE_ ? False : Indeterminate; }

    /// computes the logical conjunction of two tribools
    constexpr inline Tribool 
    operator&&(Tribool x, Tribool y) { return bool(!x) || bool(!y) ? False : bool(x) && bool(y) ? True : Indeterminate; }

    /// overload
    constexpr inline Tribool 
    operator&&(Tribool x, bool y) { return y ? x : False; }

    /// overload
    constexpr inline Tribool 
    operator&&(bool x, Tribool y) { return x ? y : False; }

    /// computes the logical disjunction of two tribools
    constexpr inline Tribool 
    operator||(Tribool x, Tribool y) { return bool(!x) && bool(!y) ? False : bool(x) || bool(y) ? True : Indeterminate; }

    /// overload
    constexpr inline Tribool 
    operator||(Tribool x, bool y) { return y ? True : x; }

    /// overload
    constexpr inline Tribool 
    operator||(bool x, Tribool y) { return x ? True : y; }

    /// compare tribools for equality
    constexpr inline Tribool
    operator==(Tribool x, Tribool y) { return is_indeterminate(x) || is_indeterminate(y) ? Indeterminate : (x && y) || (!x && !y); }

    /// overload
    constexpr inline Tribool
    operator==(Tribool x, bool y) { return x == Tribool(y); }

    /// overload
    constexpr inline Tribool
    operator==(bool x, Tribool y) { return y == Tribool(x); }

    /// compare tribools for inequality
    constexpr inline Tribool
    operator!=(Tribool x, Tribool y) { return is_indeterminate(x) || is_indeterminate(y) ? Indeterminate : !((x && y) || (!x && !y)); }

    /// overload
    constexpr inline Tribool
    operator!=(Tribool x, bool y) { return x != Tribool(y); }

    /// overload
    constexpr inline Tribool
    operator!=(bool x, Tribool y) { return y != Tribool(x); }

    /// disable none type comparison
    inline void operator==(Tribool, None) = delete;
    inline void operator==(None, Tribool) = delete;
    inline void operator!=(Tribool, None) = delete;
    inline void operator!=(None, Tribool) = delete;
}

#endif