#ifndef ETL_PLACEHOLDER_H
#define ETL_PLACEHOLDER_H

#include "etl/utility.h"

namespace Project::etl::placeholder {

    struct Retval {
        template <typename T, typename = disable_if_t<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator=(T&& other) const {
            return [captured = etl::forward<T>(other)] {
                return etl::move(captured);
            };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator=(const Argument<UF>& other) const {
            return other;
        }
    };

    inline constexpr Retval retval;

    template <typename UnaryFunction>
    struct Argument {
        UnaryFunction fn; ///< base function

        /// default constructor
        constexpr explicit Argument(UnaryFunction&& fn) : fn{etl::forward<UnaryFunction>(fn)} {}

        /// function call operator
        template <typename... Ts>
        constexpr decltype(auto) operator()(Ts&&... args) const { return fn(etl::forward_as_tuple(etl::forward<Ts>(args)...)); }

        /// assignment operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) = other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) = other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// negation operator
        [[nodiscard]] constexpr auto operator!() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return !fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// equality operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator==(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) == other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator==(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) == other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// inequality operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator!=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) != other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator!=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) != other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// greater equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator>=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) >= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator>=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) >= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// less equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator<=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) <= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator<=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) <= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// greater operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator>(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) > other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator>(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) > other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// less operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator<(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) < other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator<(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) < other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// bitwise negation operator
        [[nodiscard]] constexpr auto operator~() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return ~fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// bitwise or operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator|(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) | other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator|(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) | other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// or operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator||(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) || other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator||(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) || other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// or equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator|=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) |= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator|=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) |= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// bitwise and operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator&(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) & other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator&(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) & other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// and operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator&&(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) && other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator&&(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) && other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// and equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator&=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) &= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator&=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) &= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// multiplication operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator*(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) * other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator*(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) * other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// addition operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator+(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) + other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator+(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) + other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// division operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator/(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) / other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator/(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) / other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// subtraction operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator-(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) - other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator-(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) - other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// modulo operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator%(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) % other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator%(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) % other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// multiplication equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator*=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) *= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator*=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) *= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// addition equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator+=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) += other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator+=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) += other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// division equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator/=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) /= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator/=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) /= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// subtraction equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator-=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) -= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator-=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) -= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// modulo equal operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator%=(T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) %= other_;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator%=(const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg)) %= other.fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// dereference operator
        [[nodiscard]] constexpr auto operator*() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return *fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// address operator
        [[nodiscard]] constexpr auto operator&() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return &fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// prefix increment
        [[nodiscard]] constexpr auto operator++() const  {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return ++fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// prefix decrement
        [[nodiscard]] constexpr auto operator--() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return --fn(etl::forward<decltype(arg)>(arg));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// postfix increment
        [[nodiscard]] constexpr auto operator++(int) const  {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg))++;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// postfix decrement
        [[nodiscard]] constexpr auto operator--(int) const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg))--;
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /// subscript operator
        template <typename T, typename = disable_if<is_arg_v<decay_t<T>>>>
        [[nodiscard]] constexpr auto operator[](T&& other) const {
            auto res = [this, other_ = etl::forward<T>(other)](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg))[other_];
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <typename UF>
        [[nodiscard]] constexpr auto operator[](const Argument<UF>& other) const {
            auto res = [this, &other](auto&& arg) -> decltype(auto) {
                return fn(etl::forward<decltype(arg)>(arg))[other.fn(etl::forward<decltype(arg)>(arg))];
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        /* common member functions */

        [[nodiscard]] constexpr auto begin() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::begin(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto end() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::end(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto len() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::len(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto next() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::next(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto iter() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::iter(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        [[nodiscard]] constexpr auto reversed() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::reversed(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }

        template <size_t N>
        [[nodiscard]] constexpr auto get() const {
            auto res = [this](auto&& arg) -> decltype(auto) {
                return etl::get<N>(fn(etl::forward<decltype(arg)>(arg)));
            };
            return Argument<decltype(res)> { etl::move(res) };
        }
    };

    template <size_t N>
    constexpr Argument arg {
        [] (auto&& arg) -> decltype(auto) {
            return std::forward<decltype(etl::get<N - 1>(arg))>(etl::get<N - 1>(arg));
        }
    };

    constexpr Argument _1 {
        [] (auto&& arg) -> decltype(auto) {
            return std::forward<decltype(etl::get<0>(arg))>(etl::get<0>(arg));
        }
    };

    constexpr Argument _2 {
        [] (auto&& arg) -> decltype(auto) {
            return std::forward<decltype(etl::get<1>(arg))>(etl::get<1>(arg));
        }
    };

    constexpr Argument _3 {
        [] (auto&& arg) -> decltype(auto) {
            return std::forward<decltype(etl::get<2>(arg))>(etl::get<2>(arg));
        }
    };
}

#endif //ETL_PLACEHOLDER_H
