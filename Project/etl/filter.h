#ifndef ETL_FILTER_H
#define ETL_FILTER_H

#include "etl/iter.h"

namespace Project::etl {

    template <typename Sequence, typename UnaryPredicate>
    class Filter {
        Sequence sequence;
        UnaryPredicate fn;

    public:
        constexpr Filter(Sequence sequence, UnaryPredicate fn) : sequence(sequence), fn(fn) {}

        constexpr Filter begin() const { return *this; }
        constexpr Filter end()   const { return *this; }
        constexpr Filter iter()  const { return *this; }

        constexpr explicit operator bool() const { return bool(sequence); }

        constexpr bool operator!=(const Filter&) const { return operator bool(); }
        
        constexpr void operator++() { ++sequence; }

        constexpr decltype(auto) operator*() { 
            while (operator bool()) {
                if (fn(*sequence)) return *sequence;
                ++sequence;
            }
            return *sequence;
        }

        constexpr auto operator()() {   
            using R = remove_reference_t<decltype(*sequence)>;
            auto valid = operator bool();
            R res = valid ? operator*() : R{};
            if (valid) operator++();
            return res;
        }
    };

    /// create filter object from iterator
    template <typename Iterator, typename UnaryPredicate> constexpr auto
    filter(Iterator first, Iterator last, UnaryPredicate&& fn, int step = 1) {
        return Filter(etl::iter(first, last, step), etl::forward<UnaryPredicate>(fn));
    }

    /// create filter object from a sequence
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    filter(Sequence&& seq, UnaryPredicate&& fn) { return Filter(etl::iter(seq), etl::forward<UnaryPredicate>(fn)); }

    template <typename UnaryPredicate>
    struct FilterFunction { UnaryPredicate fn; };

    /// create filter function
    template <typename UnaryPredicate> constexpr auto
    filter(UnaryPredicate&& fn) { return FilterFunction<UnaryPredicate> { etl::forward<UnaryPredicate>(fn) }; }

    /// pipe operator to apply filter function to a sequence
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    operator|(Sequence&& seq, const FilterFunction<UnaryPredicate>& fn) { return etl::filter(etl::forward<Sequence>(seq), fn.fn); }

    /// remove extent
    template <typename Sequence, typename UnaryPredicate>
    struct remove_extent<Filter<Sequence, UnaryPredicate>> {
        typedef decltype(etl::next(etl::declval<Filter<Sequence, UnaryPredicate>>())) type;
    };

    template <typename Sequence, typename UnaryPredicate>
    struct remove_extent<const Filter<Sequence, UnaryPredicate>> {
        typedef remove_extent_t<Filter<Sequence, UnaryPredicate>> type;
    };

    template <typename Sequence, typename UnaryPredicate>
    struct remove_extent<volatile Filter<Sequence, UnaryPredicate>> {
        typedef remove_extent_t<Filter<Sequence, UnaryPredicate>> type;
    };

    template <typename Sequence, typename UnaryPredicate>
    struct remove_extent<const volatile Filter<Sequence, UnaryPredicate>> {
        typedef remove_extent_t<Filter<Sequence, UnaryPredicate>> type;
    };
} 

#endif // ETL_FILTER_H