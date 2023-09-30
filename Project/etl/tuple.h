#ifndef ETL_TUPLE_H
#define ETL_TUPLE_H

#include "etl/utility_basic.h"

namespace Project::etl {
    /// contains the actual value for one item in the tuple.
    /// @tparam i allows the get function to find the value in O(1) time
    template <size_t i, typename Item> struct TupleHead { Item item; };

    /// acts as proxy for the final class that has an extra template parameter `i`.
    template <size_t i, typename... Items> struct TupleImpl;

    /// empty tuple
    template <size_t i> struct TupleImpl<i> {};

    /// recursive specialization
    template <size_t i, typename Item, typename... Items>
    struct TupleImpl<i, Item, Items...> : public TupleHead<i, Item>, public TupleImpl<i + 1, Items...> {};

    /// final class, fixed size container, which holds elements of possibly different types
    template <typename... Items> using
    Tuple = TupleImpl<0, Items...>;

    /// create tuple with variadic template function
    template <typename... Items> constexpr auto
    tuple(Items&&... items) { return Tuple<Items...> { etl::forward<Items>(items)... }; }

    /// len specifier
    template <typename... T> constexpr size_t
    len(const Tuple<T...>&) { return sizeof...(T); }

    /// obtain a reference to i-th item in a tuple
    template <size_t i, typename Item, typename... Items> constexpr auto&
    get(TupleImpl<i, Item, Items...>& t) { return t.TupleHead<i, Item>::item; }

    template <size_t i, typename Item, typename... Items> constexpr auto&&
    get(TupleImpl<i, Item, Items...>&& t) { return etl::move(etl::get(t)); }

    template <size_t i, typename Item, typename... Items> constexpr const auto&
    get(const TupleImpl<i, Item, Items...>& t) { return t.TupleHead<i, Item>::item; }

    template <size_t i, typename Item, typename... Items> constexpr const auto&&
    get(const TupleImpl<i, Item, Items...>&& t) { return etl::move(etl::get(t)); }

    template <size_t... i, typename T> auto
    tuple_slice(T& t, integer_sequence<size_t, i...>) { return etl::tuple(etl::get<i>(t)...); }

    template <size_t Start, size_t End, typename... Args> auto
    get(const Tuple<Args...>& t) {
        static_assert(Start <= End, "Start cannot be greater than End");
        static_assert(End <= sizeof...(Args), "End cannot be greater than number or Args");
        return etl::tuple_slice(t, etl::make_range_sequence<Start, End>());
    }

    /// pair of values with possible different types
    template <typename X, typename Y = X>
    struct Pair {
        X x; Y y;
        constexpr bool operator==(const Pair& other) const { return x == other.x && y == other.y; }
        constexpr bool operator!=(const Pair& other) const { return !operator==(other); }
    };

    /// create pair, types are deduced
    template <typename X, typename Y> constexpr auto
    pair(const X& x, const Y& y) { return Pair<X, Y>{x, y}; }

    /// create empty pair
    template <typename X, typename Y = X> constexpr auto
    pair() { return Pair<X, Y>{}; }

    /// type traits
    template <typename T> struct is_pair : false_type {};
    template <typename X, typename Y> struct is_pair<Pair<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const Pair<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<volatile Pair<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const volatile Pair<X, Y>> : true_type {};
    template <typename T> inline constexpr bool is_pair_v = is_pair<T>::value;

    /// get specifier
    template <size_t i, typename X, typename Y> constexpr auto&
    get(Pair<X, Y>& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }
    template <size_t i, typename X, typename Y> constexpr auto&&
    get(Pair<X, Y>&& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }
    template <size_t i, typename X, typename Y> constexpr auto&
    get(const Pair<X, Y>& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }
    template <size_t i, typename X, typename Y> constexpr auto&&
    get(const Pair<X, Y>&& p) {
        static_assert(i < 2, "index has to be less than 2");
        if constexpr (i == 0) return p.x;
        else return p.y;
    }

    /// len specifier
    template <typename X, typename Y> constexpr size_t
    len(const Pair<X, Y>&) { return 2; }

    /// triple of values with possible different types
    template <class X, class Y = X, class Z = Y>
    struct Triple {
        X x; Y y; Z z;
        constexpr bool operator==(const Triple& other) const { return x == other.x && y == other.y && z == other.z; }
        constexpr bool operator!=(const Triple& other) const { return !operator==(other); }
    };

    /// create triple, types are deduced
    template <class X, class Y, class Z> constexpr auto
    triple(const X& x, const Y& y, const Z& z) { return Triple<X, Y, Z>{x, y, z}; }

    /// create empty triple
    template <class X, class Y = X, class Z = Y> constexpr auto
    triple() { return Triple<X, Y, Z>{}; }

    /// type traits
    template <typename T> struct is_triple : false_type {};
    template <typename X, typename Y, typename Z> struct is_triple<Triple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const Triple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<volatile Triple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const volatile Triple<X, Y, Z>> : true_type {};
    template <typename T> inline constexpr bool is_triple_v = is_triple<T>::value;

    /// get specifier
    template <size_t i, class X, class Y, class Z> constexpr auto&
    get(Triple<X, Y, Z>& p) {
        static_assert(i < 3, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }
    template <size_t i, class X, class Y, class Z> constexpr auto&&
    get(Triple<X, Y, Z>&& p) {
        static_assert(i < 3, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }
    template <size_t i, class X, class Y, class Z> constexpr auto&
    get(const Triple<X, Y, Z>& p) {
        static_assert(i < 3, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }
    template <size_t i, class X, class Y, class Z> constexpr auto&&
    get(const Triple<X, Y, Z>&& p) {
        static_assert(i < 3, "index has to be less than 3");
        if constexpr (i == 0) return p.x;
        if constexpr (i == 1) return p.y;
        else return p.z;
    }

    /// len specifier
    template <typename X, typename Y, typename Z> constexpr size_t
    len(const Triple<X, Y, Z>&) { return 3; }

    /// type_traits
    template <typename T> struct is_tuple : false_type {};
    template <typename... T> struct is_tuple<Tuple<T...>> : true_type {};
    template <typename... T> struct is_tuple<const Tuple<T...>> : true_type {};
    template <typename... T> struct is_tuple<volatile Tuple<T...>> : true_type {};
    template <typename... T> struct is_tuple<const volatile Tuple<T...>> : true_type {};
    template <typename T> inline constexpr bool is_tuple_v = is_tuple<T>::value;

    template <typename X, typename Y> struct is_pair<Tuple<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const Tuple<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<volatile Tuple<X, Y>> : true_type {};
    template <typename X, typename Y> struct is_pair<const volatile Tuple<X, Y>> : true_type {};

    template <typename X, typename Y, typename Z> struct is_triple<Tuple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const Tuple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<volatile Tuple<X, Y, Z>> : true_type {};
    template <typename X, typename Y, typename Z> struct is_triple<const volatile Tuple<X, Y, Z>> : true_type {};

    template <typename T, typename U, size_t i, size_t N> constexpr bool
    tuple_eq(const T& t, const U& u) {
        if constexpr (i >= N) return true;
        else return etl::get<i>(t) == etl::get<i>(u) && etl::tuple_eq<T, U, i + 1, N>(t, u);
    }

    /// compare operators
    template <typename... T, typename... U> constexpr bool
    operator==(const Tuple<T...>& t, const Tuple<U...>& u) {
        static_assert(sizeof...(T) == sizeof...(U), "tuple objects can only be compared if they have equal sizes.");
        return etl::tuple_eq<Tuple<T...>, Tuple<U...>, 0, sizeof...(T)>(t, u);
    }

    template <typename... T, typename... U> constexpr bool
    operator!=(const Tuple<T...>& t, const Tuple<U...>& u) { return !operator==(t, u); }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator==(const Tuple<X1, Y1>& t, const Pair<X2, Y2>& p) { return etl::get<0>(t) == p.x && etl::get<1>(t) == p.y; }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator!=(const Tuple<X1, Y1>& t, const Pair<X2, Y2>& p) { return !operator==(t, p); }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator==(const Pair<X1, Y1>& p, const Tuple<X2, Y2>& t) { return etl::get<0>(t) == p.x && etl::get<1>(t) == p.y; }

    template <typename X1, typename Y1, typename X2, typename Y2> constexpr bool
    operator!=(const Pair<X1, Y1>& p, const Tuple<X2, Y2>& t) { return !operator==(p, t); }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator==(const Tuple<X1, Y1, Z1>& t, const Triple<X2, Y2, Z2>& p)
    { return etl::get<0>(t) == p.x && etl::get<1>(t) == p.y && etl::get<2>(t) == p.z; }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator!=(const Tuple<X1, Y1, Z1>& t, const Triple<X2, Y2, Z2>& p) { return !operator==(t, p); }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator==(const Triple<X1, Y1, Z1>& p, const Tuple<X2, Y2, Z2>& t)
    { return etl::get<0>(t) == p.x && etl::get<1>(t) == p.y && etl::get<2>(t) == p.z; }

    template <typename X1, typename Y1, typename Z1, typename X2, typename Y2, typename Z2> constexpr bool
    operator!=(const Triple<X1, Y1, Z1>& t, const Tuple<X2, Y2, Z2>& p) { return !operator==(t, p); }

    /// apply a function that takes tuple as the argument
    template <typename F, typename T, size_t... i> constexpr decltype(auto)
    apply_helper_(F&& fn, T&& t, index_sequence<i...>) { return fn(etl::get<i>(t)...); }

    template <typename F, typename... T> constexpr decltype(auto)
    apply(F&& fn, Tuple<T...>&& t) { 
        return apply_helper_(etl::forward<F>(fn), etl::forward<Tuple<T...>>(t), index_sequence_for<T...>{}); 
    }
    
    template <typename F, typename X, typename Y> constexpr decltype(auto)
    apply(F&& fn, Pair<X, Y>&& t) { 
        return apply_helper_(etl::forward<F>(fn), etl::forward<Pair<X, Y>>(t), make_index_sequence<2>{}); 
    }

    template <typename F, typename X, typename Y, typename Z> constexpr decltype(auto)
    apply(F&& fn, Triple<X, Y, Z>&& t) { 
        return apply_helper_(etl::forward<F>(fn), etl::forward<Triple<X, Y, Z>>(t), make_index_sequence<3>{}); 
    }
    /// forward_as_tuple
    template <typename... Items> constexpr Tuple<Items&&...>
    forward_as_tuple(Items&&... items) noexcept { return Tuple<Items&&...>{etl::forward<Items>(items)...}; }
}

// some specializations to enable structure binding
#include <utility>
namespace std {

    template <typename... Items>
    struct tuple_size<Project::etl::Tuple<Items...>> : public Project::etl::integral_constant<size_t, sizeof...(Items)> {};

    template <size_t i, typename Item, typename... Items>
    struct tuple_element<i, Project::etl::Tuple<Item, Items...>> : tuple_element<i - 1, Project::etl::Tuple<Items...>> {};

    template <typename Item, typename... Items>
    struct tuple_element<0, Project::etl::Tuple<Item, Items...>> { typedef Item type; };

    template <size_t i>
    struct tuple_element<i, Project::etl::Tuple<>> { static_assert(i < tuple_size<Project::etl::Tuple<>>::value); };
}

#endif //ETL_TUPLE_H
