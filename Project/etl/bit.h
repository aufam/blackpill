#ifndef ETL_BIT_H
#define ETL_BIT_H

#include "etl/type_traits.h"
#include <cstring> // memcpy

namespace Project::etl {

    /// reinterpret the object representation of one type as that of another
    template <typename TDestination, typename TSource,
            typename = enable_if_t<is_same_size_v<TDestination, TSource>>> auto
    bit_cast(const TSource& source) {
        TDestination destination;
        memcpy(&destination, &source, sizeof(TDestination));
        return destination;
    }

    /// counts the number of consecutive 0 or 1 bits, starting from the least significant bit
    template <size_t N, typename T,
            typename = enable_if_t<(N == 0u || N == 1u) && is_unsigned_integral_v<T> && sizeof(T) <= 4u>> constexpr uint32_t
    count_trailing(T value) {
        if ((value & 0b1u) == (N ? 0u : 1u)) return 0u;

        uint32_t count = 1u;
        if constexpr (sizeof(T) == 4u) {
            if ((value & 0b1111'1111'1111'1111u) == (N ? 0b1111'1111'1111'1111u : 0u)) {
                value >>= 16u;
                count += 16u;
            }
        }
        if constexpr (sizeof(T) >= 2u) {
            if ((value & 0b1111'1111u) == (N ? 0b1111'1111u : 0u)) {
                value >>= 8u;
                count += 8u;
            }
        }
        if ((value & 0b1111u) == (N ? 0b1111u : 0u)) {
            value >>= 4u;
            count += 4u;
        }
        if ((value & 0b11u) == (N ? 0b11u : 0u)) {
            value >>= 2u;
            count += 2u;
        }
        if ((value & 0b1u) == (N ? 0b1u : 0u)) {
            value >>= 1u;
            count += 1u;
        }
        count -= (value & 0b1u) == (N ? 0u : 1u);
        return count;
    }

    template <typename T> auto
    count_trailing_zeros(T value) { return etl::count_trailing<0>(value); }

    template <typename T> auto
    count_trailing_ones(T value) { return etl::count_trailing<1>(value); }

    /// counts the number of consecutive 0 or 1 bits, starting from the most significant bit
    template <size_t N, typename T,
            typename = enable_if_t<(N == 0u || N == 1u) && is_unsigned_integral_v<T> && sizeof(T) <= 4u, uint32_t>> constexpr uint32_t
    count_leading(T value) {
        if constexpr (sizeof(T) == 4u) {
            if ((value & 0b1000'0000'0000'0000'0000'0000'0000'0000u) == (N ? 0u : 0b1000'0000'0000'0000'0000'0000'0000'0000u)) return 0u;
        }
        else if constexpr (sizeof(T) == 2u) {
            if ((value & 0b1000'0000'0000'0000u) == (N ? 0u : 0b1000'0000'0000'0000u)) return 0u;
        }
        else if constexpr (sizeof(T) == 1u) {
            if ((value & 0b1000'0000u) == (N ? 0u : 0b1000'0000u)) return 0u;
        }

        uint32_t count = 1u;
        if constexpr (sizeof(T) == 4u) {
            if ((value & 0b1111'1111'1111'1111'0000'0000'0000'0000u) == (N ? 0b1111'1111'1111'1111'0000'0000'0000'0000u : 0u)) {
                value <<= 16u;
                count += 16u;
            }
        }
        if constexpr (sizeof(T) >= 2u) {
            if ((value & 0b1111'1111'0000'0000'0000'0000'0000'0000u) == (N ? 0b1111'1111'0000'0000'0000'0000'0000'0000u : 0u)) {
                value <<= 8u;
                count += 8u;
            }
        }
        if ((value & 0b1111'0000'0000'0000'0000'0000'0000'0000u) == (N ? 0b1111'0000'0000'0000'0000'0000'0000'0000u : 0u)) {
            value <<= 4u;
            count += 4u;
        }
        if ((value & 0b1100'0000'0000'0000'0000'0000'0000'0000u) == (N ? 0b1100'0000'0000'0000'0000'0000'0000'0000u : 0u)) {
            value <<= 2u;
            count += 2u;
        }
        if ((value & 0b1000'0000'0000'0000'0000'0000'0000'0000u) == (N ? 0b1000'0000'0000'0000'0000'0000'0000'0000u : 0u)) {
            value <<= 1u;
            count += 1u;
        }
        count -= (value & 0b1000'0000'0000'0000'0000'0000'0000'0000u) == (N ? 0u : 0b1000'0000'0000'0000'0000'0000'0000'0000u);
        return count;
    }

    template <typename T> auto
    count_leading_zero(T value) { return etl::count_leading<0>(value); }

    template <typename T> auto
    count_leading_ones(T value) { return etl::count_leading<1>(value); }

    /// finds the smallest number of bits needed to represent the given value
    template <typename T,
            typename = enable_if_t<is_unsigned_integral_v<T> && sizeof(T) <= 4u>> constexpr uint32_t
    bit_width(T value) { return sizeof(T) * 8 - etl::count_leading_zero(value); }

    /// counts the number of 1 bits in an unsigned integer
    template <typename T,
            typename = enable_if_t<is_unsigned_integral_v<T> && sizeof(T) <= 4u>> constexpr uint32_t
    count_bits(T value) {
        uint32_t count = value - ((value >> 1U) & 0x5555'5555u);
        count = ((count >> 2U) & 0x3333'3333u) + (count & 0x3333'3333u);
        count = ((count >> 4U)  + count) & 0x0F0F'0F0Fu;
        count = ((count >> 8U)  + count) & 0x00FF'00FFu;
        count = ((count >> 16U) + count) & 0x0000'00FFu;
        return count;
    }

    /// checks if a number is an integral power of two
    template <typename T, typename = enable_if_t<is_unsigned_integral_v<T>>> constexpr bool
    has_single_bit(T value) { return (value & (value - 1)) == 0u; }

    /// finds the smallest integral power of two not less than the given value
    template <typename T, typename = enable_if_t<is_unsigned_integral_v<T>>> constexpr T
    bit_ceil(T value) { return value == T(0) ? T(1) : T(1) << etl::bit_width(T(value - T(1))); }

    /// finds the largest integral power of two not greater than the given value
    template <typename T, typename = enable_if_t<is_unsigned_integral_v<T>>> constexpr T
    bit_floor(T value) { return value == T(0) ? T(0) : T(1) << (etl::bit_width(value) - T(1)); }

    /// computes the result of bitwise left-rotation
    template <typename T, typename = enable_if_t<is_integral_v<T>>> constexpr T
    rotate_left(T value, size_t distance) {
        size_t bits = sizeof (T) * 8;
        distance %= bits;
        size_t shift = bits - distance;
        if (shift == bits) return value;
        return (value << distance) | (value >> shift);
    }

    /// computes the result of bitwise right-rotation
    template <typename T, enable_if_t<is_integral_v<T>>> constexpr T
    rotate_right(T value, size_t distance) {
        size_t bits = sizeof (T) * 8;
        distance %= bits;
        size_t shift = bits - distance;
        if (shift == bits) return value;
        return (value >> distance) | (value << shift);
    }

    /// parity. 0 = even, 1 = odd
    template <typename T, enable_if_t<is_unsigned_integral_v<T> && sizeof(T) <= 4u>> constexpr uint32_t
    parity(T value) {
        if constexpr (sizeof(T) == 4u) value ^= value >> 16u;
        if constexpr (sizeof(T) >= 2u) value ^= value >> 8u;
        value ^= value >> 4u;
        value &= 0x0Fu;
        return (0x6996u >> value) & 1u;
    }

    template <typename T, typename = enable_if_t<is_unsigned_integral_v<T> && sizeof(T) <= 4u>> constexpr bool
    is_odd_parity(T value) { return etl::parity(value) == 1u; }

    template <typename T, typename = enable_if_t<is_unsigned_integral_v<T> && sizeof(T) <= 4u>> bool
    is_even_parity(T value) { return etl::parity(value) == 0u; }

    template <typename T, typename = enable_if_t<is_integral_v<T>>> bool
    is_odd(T value) { return (value & 1u) != 0u; }

    template <typename T, typename = enable_if_t<is_integral_v<T>>> constexpr bool
    is_even(T value) { return (value & 1u) == 0u; }
}

#endif //ETL_BIT_H
