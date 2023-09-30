#ifndef ETL_ITER_H
#define ETL_ITER_H

#include "etl/utility_basic.h"

namespace Project::etl {
    
    /// python-like iter
    template <typename Iterator>
    class Iter {
        static_assert(is_iterator_v<Iterator>, "The type has to be an iterator");
        Iterator first, last;
        int step;

    public:
        constexpr Iter(Iterator first, Iterator last, int step = 1) 
            : first(first)
            , last(last)
            , step(step) {}

        constexpr Iter begin() const { return *this; }
        constexpr Iter end()   const { return *this; }
        constexpr Iter iter()  const { return *this; }

        constexpr size_t len() const {
            if constexpr (has_operator_minus_v<Iterator>) {
                return step == 0 ? 0
                        : step > 0 ? (last - first) / step
                        : (first - last) / (-step);
            } else {
                size_t cnt = 0;
                for (Iterator it = first; it != last; it = increment_helper(step))
                    ++cnt;
                return cnt;
            }
        }

        constexpr decltype(auto) operator[](int i) { 
            if (i < 0) i = len() + i; // allowing negative index
            if (i < 0) return *reinterpret_cast<remove_reference_t<decltype(*first)>*>(nullptr);
            return *increment_helper(step * i);
        }

        constexpr decltype(auto) operator[](int i) const { 
            if (i < 0) i = len() + i; // allowing negative index
            if (i < 0) return *reinterpret_cast<const remove_reference_t<decltype(*first)>*>(nullptr);
            return *increment_helper(step * i);
        }

        constexpr explicit operator bool() const { 
            return step != 0 && first != last; 
        }

        constexpr bool operator!=(const Iter&) const { 
            return operator bool(); 
        }

        constexpr void operator++() {
            first = increment_helper(step);
        }

        constexpr decltype(auto) operator*() { 
            return *first; 
        }

        constexpr auto operator()() {
            using R = remove_reference_t<decltype(*first)>;
            auto valid = operator bool();
            R res = valid ? *first : R{};
            if (valid) operator++();
            return res;
        }

    private:
        constexpr Iterator increment_helper(int inc) const {
            Iterator res = first;
            if (inc > 0) {
                for (int i = 0; i != inc; i += 1) {
                    if (res == last) break;
                    ++res;
                }
            } else if (inc < 0) {
                for (int i = 0; i != -inc; i += 1) {
                    if (res == last)
                        break;
                    if constexpr (has_prefix_decrement_v<Iterator>)
                        --res;
                    else
                        break; // not supposed to be here
                }
            }
            return res;
        }
    };

    /// create iter object from iterator
    template <typename Iterator, typename = enable_if_t<has_prefix_decrement_v<Iterator>>> constexpr auto
    iter(Iterator first, Iterator last, int step = 1) { return Iter(first, last, step); }

    /// create iter object from a container
    template <typename Container, typename = enable_if_t<has_begin_end_v<Container>>> constexpr auto
    iter(Container& cont) {
        if constexpr (etl::has_iter_v<Container>)
            return cont.iter();
        else
            return Iter(etl::begin(cont), etl::end(cont), 1);
    }

    /// create reversed iter object from a container
    template <typename Container, typename = enable_if_t<has_begin_end_v<Container>>> constexpr auto
    reversed(Container& cont) {
        using Iterator = decltype(etl::begin(cont));
        if constexpr (has_reversed_v<Container>)
            return cont.reversed();
        else if constexpr (has_operator_minus_int_v<Iterator> && has_operator_plus_int_v<Iterator>)
            return Iter(etl::end(cont) - 1, etl::begin(cont) - 1, -1);
        else
            return Iter(cont.rbegin(), cont.rend(), 1);
    }

    /// remove extent
    template <typename T> struct remove_extent<Iter<T>> { typedef remove_reference_t<decltype(*etl::declval<T>())> type; };
    template <typename T> struct remove_extent<const Iter<T>> { typedef remove_extent_t<Iter<T>> type; };
    template <typename T> struct remove_extent<volatile Iter<T>> { typedef remove_extent_t<Iter<T>> type; };
    template <typename T> struct remove_extent<const volatile Iter<T>> { typedef remove_extent_t<Iter<T>> type; };

    /// check if all elements of two sequences are the same
    template <typename Sequence1, typename Sequence2,
            typename = enable_if_t<
                has_begin_end_v<remove_reference_t<Sequence1>> && 
                has_begin_end_v<remove_reference_t<Sequence2>> &&
                !placeholder::is_arg_v<remove_const<Sequence1>> &&
                !placeholder::is_arg_v<remove_reference_t<Sequence2>>
            >
    > constexpr bool
    operator==(Sequence1&& seq1, Sequence2&& seq2) {
        auto iter1 = etl::iter(seq1);
        auto iter2 = etl::iter(seq2);

        if constexpr (has_len_v<decltype(iter1)> && has_len_v<decltype(iter2)>)
            if (etl::len(iter1) != etl::len(iter2))
                return false;

        for (; bool(iter1) && bool(iter2); ++iter1, ++iter2) if (*iter1 != *iter2) return false;
        return !(bool(iter1) || bool(iter2));
    }
}

#endif // ETL_ITER_H