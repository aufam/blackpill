#ifndef ETL_ZIP_H
#define ETL_ZIP_H

#include "etl/tuple.h"
#include "etl/iter.h"

namespace Project::etl {

    /// python-like zip
    template <typename... Sequences>
    class Zip {
        Tuple<Sequences...> sequences;

    public:
        constexpr explicit Zip(Sequences... seq) : sequences{seq...} {}

        constexpr Zip begin() const { return *this; }
        constexpr Zip end()   const { return *this; }
        constexpr Zip iter()  const { return *this; }

        constexpr explicit operator bool() const { return bool_helper_<0>(); }

        constexpr bool operator!=(const Zip&) const { return operator bool(); }

        constexpr void operator++() { inc_helper_<0>(); }

        constexpr auto operator*() { return deref_helper_(index_sequence_for<Sequences...>{}); }

        constexpr auto operator()() { return next_helper_(index_sequence_for<Sequences...>{}); }

    private:
        template <size_t i>
        constexpr bool bool_helper_() const {
            if constexpr (i == sizeof...(Sequences)) return true;
            else return bool(etl::get<i>(sequences)) && bool_helper_<i + 1>();
        }

        template <size_t i>
        constexpr void inc_helper_() {
            if constexpr (i < sizeof...(Sequences)) {
                ++etl::get<i>(sequences);
                inc_helper_<i + 1>();
            }
        }

        template <size_t... i>
        constexpr auto deref_helper_(index_sequence<i...>) { 
            // using R = Tuple<decltype(*etl::get<i>(sequences))...>; // TODO: somehow Tuple doesn't work
            static_assert (sizeof...(i) == 2 || sizeof...(i) == 3, "currently only support binary or ternary zip");
            if constexpr (sizeof...(i) == 2)
                return Pair<decltype(*etl::get<i>(sequences))...> { *etl::get<i>(sequences)... };
            else 
                return Triple<decltype(*etl::get<i>(sequences))...> { *etl::get<i>(sequences)... };
        }

        template <size_t... i>
        constexpr auto next_helper_(index_sequence<i...>) {
            using R = Tuple<remove_reference_t<decltype(*etl::get<i>(sequences))>...>;
            auto valid = operator bool();
            auto res = valid ? R { *etl::get<i>(sequences)... } : R{}; 
            if (valid) operator++(); 
            return res; 
        }
    };

    /// create zip object from begin-end pair
    template <typename... Iterators> constexpr auto
    zip(Pair<Iterators>... begin_end) { return Zip(etl::iter(begin_end.x, begin_end.y)...); }

    /// create zip object from any sequence
    template <typename... Sequences> constexpr auto
    zip(Sequences&&... seq) { return Zip(etl::iter(seq)...); }
}

#endif // ETL_ZIP_H