#ifndef ETL_TRANSFORM_H
#define ETL_TRANSFORM_H

#include "etl/iter.h"

namespace Project::etl {

    template <typename Sequence, typename UnaryFunction>
    class Transform {
        Sequence sequence;
        UnaryFunction fn;

    public:
        constexpr Transform(Sequence sequence, UnaryFunction fn) : sequence(sequence), fn(fn) {}

        constexpr Transform begin() const { return *this; }
        constexpr Transform end()   const { return *this; }
        constexpr Transform iter()  const { return *this; }

        constexpr explicit operator bool() const { return bool(sequence); }

        constexpr bool operator!=(const Transform&) const { return operator bool(); }
        
        constexpr void operator++() { ++sequence; }

        constexpr decltype(auto) operator*() { return fn(*sequence); }

        constexpr auto operator()() { 
            using R = remove_reference_t<decltype(fn(*sequence))>;
            auto valid = operator bool();
            R res = valid ? operator*() : R{};
            if (valid) operator++();
            return res;
        }
    };

    /// create transform object from iterator
    template <typename Iterator, typename UnaryFunction> constexpr auto
    transform(Iterator first, Iterator last, UnaryFunction&& fn, int step = 1) { return Transform(etl::iter(first, last, step), etl::forward<UnaryFunction>(fn)); }

    /// create transform object from a sequence
    template <typename Sequence, typename UnaryFunction> constexpr auto
    transform(Sequence&& seq, UnaryFunction&& fn) { return Transform(etl::iter(seq), etl::forward<UnaryFunction>(fn)); }

    template <typename UnaryFunction>
    struct TransformFunction { UnaryFunction fn; };

    /// create transform function
    template <typename UnaryFunction> constexpr auto
    transform(UnaryFunction&& fn) { return TransformFunction<UnaryFunction> { etl::forward<UnaryFunction>(fn) }; }

    /// pipe operator to apply transform function to a sequence
    template <typename Sequence, typename UnaryFunction> constexpr auto
    operator|(Sequence&& seq, const TransformFunction<UnaryFunction>& fn) { return etl::transform(etl::forward<Sequence>(seq), fn.fn); }

    /// remove extent
    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<Transform<Sequence, UnaryFunction>> {
        typedef decltype(etl::next<Transform<Sequence, UnaryFunction>>(etl::declval<Transform<Sequence, UnaryFunction>>())) type;
    };

    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<const Transform<Sequence, UnaryFunction>> {
        typedef remove_extent_t<Transform<Sequence, UnaryFunction>> type;
    };

    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<volatile Transform<Sequence, UnaryFunction>> {
        typedef remove_extent_t<Transform<Sequence, UnaryFunction>> type;
    };

    template <typename Sequence, typename UnaryFunction>
    struct remove_extent<const volatile Transform<Sequence, UnaryFunction>> {
        typedef remove_extent_t<Transform<Sequence, UnaryFunction>> type;
    };
} 

#endif // ETL_TRANSFORM_H
