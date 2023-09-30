#ifndef ETL_ALGORITHM_H
#define ETL_ALGORITHM_H

#include "etl/utility.h"

namespace Project::etl {

    /// find the first element that is equal to the given value
    template <typename Iterator, typename T> constexpr auto
    find(Iterator first, Iterator last, T&& value) {
        for (; first != last; ++first) if (*first == value) 
            return first;
        return last;
    }

    /// find the first element that is equal to the given value
    template <typename Sequence, typename T> constexpr auto
    find(Sequence&& seq, T&& value) { return etl::find(etl::begin(seq), etl::end(seq), etl::forward<T>(value)); }

    /// find the first element that satisfies the given predicate
    template <typename Iterator, typename UnaryPredicate> constexpr auto
    find_if(Iterator first, Iterator last, UnaryPredicate&& fn) {
        for (; first != last; ++first) if (fn(*first)) return first;
        return last;
    }

    /// find the first element that satisfies the given predicate
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    find_if(Sequence&& seq, UnaryPredicate&& fn) { 
        return etl::find_if(etl::begin(seq), etl::end(seq), etl::forward<UnaryPredicate>(fn)); 
    }

    /// find the first element that does not satisfy the given predicate
    template <typename Iterator, typename UnaryPredicate> constexpr auto
    find_if_not(Iterator first, Iterator last, UnaryPredicate&& fn) {
        for (; first != last; ++first) if (!fn(*first)) return first;
        return last;
    }

    /// find the first element that does not satisfy the given predicate
    template <typename Sequence, typename UnaryPredicate> constexpr auto
    find_if_not(Sequence&& seq, UnaryPredicate&& fn) { 
        return etl::find_if_not(etl::begin(seq), etl::end(seq), etl::forward<UnaryPredicate>(fn)); 
    }
    
    /// check if all of the elements satisfy the given predicate
    template <typename Iterator, typename UnaryPredicate> constexpr bool
    all_if(Iterator first, Iterator last, UnaryPredicate&& fn) { 
        return etl::find_if_not(first, last, etl::forward<UnaryPredicate>(fn)) == last; 
    }

    /// check if all of the elements satisfy the given predicate
    template <typename Sequence, typename UnaryPredicate> constexpr bool
    all_if(Sequence&& seq, UnaryPredicate&& fn) { 
        return etl::all_if(etl::begin(seq), etl::end(seq), etl::forward<UnaryPredicate>(fn)); 
    }

    /// check if any element satisfies the given predicate
    template <typename Iterator, typename UnaryPredicate> constexpr bool
    any_if(Iterator first, Iterator last, UnaryPredicate&& fn) { 
        return etl::find_if(first, last, etl::forward<UnaryPredicate>(fn)) != last; 
    }

    /// check if any element satisfies the given predicate
    template <typename Sequence, typename UnaryPredicate> constexpr bool
    any_if(Sequence&& seq, UnaryPredicate&& fn) { 
        return etl::any_if(etl::begin(seq), etl::end(seq), etl::forward<UnaryPredicate>(fn));
    }
    
    /// check if none of the elements satisfy the given predicate
    template <typename Iterator, typename UnaryPredicate> constexpr bool
    none_if(Iterator first, Iterator last, UnaryPredicate&& fn) { 
        return etl::find_if(first, last, etl::forward<UnaryPredicate>(fn)) == last; 
    }

    /// check if none of the elements satisfy the given predicate
    template <typename Sequence, typename UnaryPredicate> constexpr bool
    none_if(Sequence&& seq, UnaryPredicate&& fn) { 
        return etl::none_if(etl::begin(seq), etl::end(seq), etl::forward<UnaryPredicate>(fn)); 
    }

    /// check if all of the elements are equal to a successive call of a generator
    template <typename Iterator, typename Generator> constexpr bool
    all_of(Iterator first, Iterator last, Generator&& fn) {
        for (; first != last; ++first) if (*first != fn()) return false;
        return true;
    }

    /// check if all of the elements are equal to a successive call of a generator
    template <typename Sequence, typename Generator> constexpr bool
    all_of(Sequence&& seq, Generator&& fn) { 
        return etl::all_of(etl::begin(seq), etl::end(seq), etl::forward<Generator>(fn)); 
    }

    /// check if any element is equal to a successive call of a generator
    template <typename Iterator, typename Generator> constexpr bool
    any_of(Iterator first, Iterator last, Generator&& fn) {
        for (; first != last; ++first) if (*first == fn()) return true;
        return false;
    }

    /// check if any element is equal to a successive call of a generator
    template <typename Sequence, typename Generator> constexpr bool
    any_of(Sequence&& seq, Generator&& fn) { 
        return etl::any_of(etl::begin(seq), etl::end(seq), etl::forward<Generator>(fn)); 
    }

    /// check if none of the elements are equal to successive call of a generator
    template <typename Iterator, typename Generator> constexpr bool
    none_of(Iterator first, Iterator last, Generator&& fn) {
        for (; first != last; ++first) if (*first == fn()) return false;
        return true;
    }

    /// check if none of the elements are equal to successive call of a generator
    template <typename Sequence, typename Generator> constexpr bool
    none_of(Sequence&& seq, Generator&& fn) { 
        return etl::none_of(etl::begin(seq), etl::end(seq), etl::forward<Generator>(fn)); 
    }

    /// check if all elements of two sequences are the same
    template <typename Sequence1, typename Sequence2> constexpr bool
    compare_all(Sequence1&& seq1, Sequence2&& seq2) { return operator==(etl::forward<Sequence1>(seq1), etl::forward<Sequence2>(seq2)); }

    /// check if any elements of two sequences are the same
    template <typename Sequence1, typename Sequence2> constexpr bool
    compare_any(Sequence1&& seq1, Sequence2&& seq2) {
        auto iter1 = etl::iter(seq1);
        auto iter2 = etl::iter(seq2);
        for (; bool(iter1) && bool(iter2); ++iter1, ++iter2) if (*iter1 == *iter2) return true;
        return false;
    }

    /// check if none of the elements of two sequences are the same
    template <typename Sequence1, typename Sequence2> constexpr bool
    compare_none(Sequence1&& seq1, Sequence2&& seq2) {
        auto iter1 = etl::iter(seq1);
        auto iter2 = etl::iter(seq2);
        for (; bool(iter1) && bool(iter2); ++iter1, ++iter2) if (*iter1 == *iter2) return false;
        return true;
    }

    /// applies function fn(item) to each element
    template <typename Iterator, typename UnaryFunction> constexpr auto
    foreach(Iterator first, Iterator last, UnaryFunction&& fn) {
        for (; first != last; ++first) fn(*first);
        return fn;
    }

    /// applies function fn(item) to each element
    template <typename Sequence, typename UnaryFunction> constexpr auto
    foreach(Sequence&& seq, UnaryFunction&& fn) { 
        return etl::foreach(etl::begin(seq), etl::end(seq), etl::forward<UnaryFunction>(fn)); 
    }

    /// applies function fn(item, result) to each element
    template <typename Iterator, typename UnaryFunction, typename R> constexpr auto
    fold(Iterator first, Iterator last, UnaryFunction&& fn, R& result) {
        for (; first != last; ++first) fn(*first, result);
        return fn;
    }

    /// applies function fn(item, result) to each element
    template <typename Sequence, typename UnaryFunction, typename R> constexpr auto
    fold(Sequence&& seq, UnaryFunction&& fn, R& result) { 
        return etl::fold(etl::begin(seq), etl::end(seq), etl::forward<UnaryFunction>(fn), result); 
    }
    
    /// assign the given value to each element
    template <typename Iterator, typename T> constexpr void
    fill(Iterator first, Iterator last, T&& value) { for (; first != last; ++first) *first = etl::forward<T>(value); }

    /// assign the given value to each element
    template <typename Sequence, typename T> constexpr void
    fill(Sequence&& seq, T&& value) { for (auto& x : seq) x = etl::forward<T>(value); }
    
    /// assign the results of successive call of the generator to each element
    template <typename Iterator, typename Generator> constexpr void
    generate(Iterator first, Iterator last, Generator&& fn) { for (; first != last; ++first) *first = fn(); }

    /// assign the results of successive call of the generator to each element
    template <typename Sequence, typename Generator> constexpr void
    generate(Sequence&& seq, Generator&& fn) { for (auto& x : seq) x = fn(); }

    /// return the number of elements are equal to the given value
    template <typename Iterator, typename T> constexpr int
    count(Iterator first, Iterator last, T&& value) {
        int res = 0;
        for (; first != last; ++first) if (*first == value) ++res;
        return res;
    }

    /// return the number of elements are equal to the given value
    template <typename Sequence, typename T> constexpr int
    count(Sequence&& seq, T&& value) { return etl::count(etl::begin(seq), etl::end(seq), etl::forward<T>(value)); }

    /// return the number of elements that satidfy the given predicate
    template <typename Iterator, typename UnaryPredicate> constexpr int
    count_if(Iterator first, Iterator last, UnaryPredicate&& fn) {
        int res = 0;
        for (; first != last; ++first) if (fn(*first)) ++res;
        return res;
    }

    /// return the number of elements that satidfy the given predicate
    template <typename Sequence, typename UnaryPredicate> constexpr int
    count_if(Sequence&& seq, UnaryPredicate&& fn) { 
        return etl::count_if(etl::begin(seq), etl::end(seq), etl::forward<UnaryPredicate>(fn)); 
    }

    /// copy all of elements to the given destination
    template <typename Iterator, typename IteratorDest> constexpr auto
    copy(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) *dest = *first;
        return dest;
    }

    /// copy all of elements to the given destination
    template <typename Sequence, typename SequenceDest> constexpr auto
    copy(Sequence&& seq, SequenceDest& dest) { return copy(etl::begin(seq), etl::end(seq), etl::begin(dest)); }

    /// copy all of elements that satisfy the predicate to the given destination
    template <typename Iterator, typename IteratorDest, typename UnaryPredicate> constexpr auto
    copy_if(Iterator first, Iterator last, IteratorDest dest, UnaryPredicate&& fn) {
        for (; first != last && dest != nullptr; ++first) if (fn(*first)) { *dest = *first; ++dest; }
        return dest;
    }
    
    /// copy all of elements that satisfy the predicate to the given destination
    template <typename Sequence, typename SequenceDest, typename UnaryPredicate> constexpr auto
    copy_if(Sequence&& seq, SequenceDest& dest, UnaryPredicate&& fn) {
        return etl::copy_if(etl::begin(seq), etl::end(seq), etl::begin(dest), etl::forward<UnaryPredicate>(fn));
    }

    /// move all of elements that satisfy the predicate to the given destination
    template <typename Iterator, typename IteratorDest> constexpr auto
    move(Iterator first, Iterator last, IteratorDest dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) *dest = etl::move(*first);
        return dest;
    }

    /// move all of elements that satisfy the predicate to the given destination
    template <typename Sequence, typename SequenceDest> constexpr void
    move(Sequence&& seq, SequenceDest& dest) {
        for (auto [x, y] : etl::zip(etl::forward<Sequence>(seq), etl::forward<SequenceDest>(dest)))
            y = etl::move(x);
    }

    /// swap all of elements
    template <typename Iterator1, typename Iterator2> constexpr void
    swap_element(Iterator1 first, Iterator1 last, Iterator2 dest) {
        for (; first != last && dest != nullptr; ++first, ++dest) etl::swap(*first, *dest);
    }
    
    /// swap all of elements
    template <typename Sequence1, typename Sequence2> constexpr void
    swap_element(Sequence1&& seq1, Sequence2&& seq2) {
        for (auto [x, y] : etl::zip(etl::forward<Sequence1>(seq1), etl::forward<Sequence2>(seq2)))
            etl::swap(x, y);
    }

    /// replace all elements with the given value
    template <typename Iterator, typename T> constexpr void
    replace(Iterator first, Iterator last, T&& old, T&& value) {
        for (; first != last; ++first) if (*first == old) *first = etl::forward<T>(value);
    }

    /// replace all elements with the given value
    template <typename Sequence, typename T> constexpr void
    replace(Sequence&& seq, T&& old, T&& value) { 
        etl::replace(etl::begin(seq), etl::end(seq), etl::forward<T>(old), etl::forward<T>(value)); 
    }

    /// replace all elements satisfy the predicate with the given value
    template <typename Iterator, typename UnaryPredicate, typename T> void
    replace_if(Iterator first, Iterator last, UnaryPredicate&& fn, T&& value) {
        for (; first != last; ++first) if (fn(*first)) *first = etl::forward<T>(value);
    }
    
    /// replace all elements satisfy the predicate with the given value
    template <typename Sequence, typename UnaryPredicate, typename T> constexpr void
    replace_if(Sequence&& seq, UnaryPredicate&& fn, T&& value) {
        etl::replace_if(etl::begin(seq), etl::end(seq), fn, etl::forward<T>(value));
    }

    /// return the greater of the given values
    template <typename T1, typename T2, typename... Tn> constexpr auto
    max(T1&& val1, T2&& val2, Tn&&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 > val2 ? val1 : val2;
        else return etl::max(etl::max(etl::forward<T1>(val1), etl::forward<T2>(val2)), etl::forward<Tn>(vals)...);
    }

    /// return the smaller of the given values
    template <typename T1, typename T2, typename... Tn> constexpr auto
    min(T1&& val1, T2&& val2, Tn&&... vals) {
        if constexpr (sizeof...(vals) == 0) return val1 < val2 ? val1 : val2;
        else return etl::min(etl::min(etl::forward<T1>(val1), etl::forward<T2>(val2)), etl::forward<Tn>(vals)...);
    }

    /// return the sum of all arguments
    template <typename T, typename... Ts> constexpr auto
    sum(T&& val, Ts&&... vals) {
        if constexpr (sizeof...(vals) == 0) return val;
        else return val + etl::sum(etl::forward<Ts>(vals)...);
    }

    /// returns the largest element in a sequence
    template <typename Iterator> constexpr auto
    max_element(Iterator first, Iterator last) {
        if (first == last) return *last;
        auto largest = first;
        for (++first; first != last; ++first) if (*first > *largest) largest = first;
        return *largest;
    }

    /// returns the largest element in a sequence
    template <typename Sequence> constexpr auto
    max_element(Sequence&& seq) { return etl::max_element(etl::begin(seq), etl::end(seq)); }

    /// returns the smallest element in a sequence
    template <typename Iterator> constexpr auto
    min_element(Iterator first, Iterator last) {
        if (first == last) return *last;
        auto smallest = first;
        for (++first; first != last; ++first) if (*first < *smallest) smallest = first;
        return *smallest;
    }

    /// returns the smallest element in a sequence
    template <typename Sequence> constexpr auto
    min_element(Sequence&& seq) { return etl::min_element(etl::begin(seq), etl::end(seq)); }

    /// returns the sum of all elements in a sequence
    template <typename Iterator> constexpr auto
    sum_element(Iterator first, Iterator last) {
        using T = remove_reference_t<decltype(*first)>;
        using R = conditional_t<is_arithmetic_v<T>, conditional_t<is_floating_point_v<T>, double, long>, T>;
        using S = conditional_t<is_unsigned_v<R>, add_unsigned_t<R>, R>;
        
        if (first == last) return S{};
        S res = *first;
        for (++first; first != last; ++first) res += *first;
        return res;
    }

    /// returns the sum of all elements in a sequence
    template <typename Sequence> constexpr auto
    sum_element(Sequence&& seq) { return etl::sum_element(etl::begin(seq), etl::end(seq)); }

    /// clamps a value between a pair of boundary values
    template <typename T, typename U, typename V> constexpr auto
    clamp(T&& x, U&& lo, V&& hi) {
        auto low = etl::min(etl::forward<U>(lo), etl::forward<V>(hi));
        auto high = etl::max(etl::forward<U>(lo), etl::forward<V>(hi));
        return x > high ? high : x < low ? low : x;
    }
}

#endif //ETL_ALGORITHM_H
