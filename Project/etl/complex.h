#ifndef ETL_COMPLEX_H
#define ETL_COMPLEX_H

#include "etl/math.h"
#include "etl/numerics.h"

namespace Project::etl {
    /// complex number with integer type. real and image represent their amplitude
    template <typename T> struct complex;
    using complex8  = complex<int8_t>;
    using complex16 = complex<int16_t>;
    using complex32 = complex<int32_t>;

    template <class T>
    struct complex {
        static constexpr bool is_complex32 = is_same_v<T, int32_t>;
        static constexpr bool is_complex16 = is_same_v<T, int16_t>;
        static constexpr bool is_complex8  = is_same_v<T, int8_t >;
        static_assert(is_complex32 || is_complex16 || is_complex8, "type has to be signed integral");

        typedef T value_type;

        T real, imag;
        
        constexpr explicit complex(int32_t real, int32_t imag = 0)  : real(real), imag(imag) {}
        constexpr explicit complex(float real, float imag = 0)      : real(from_float_(real)), imag(from_float_(imag)) {}
        constexpr explicit complex(double real, double imag = 0)    : real(from_float_(real)), imag(from_float_(imag)) {}
        constexpr complex() : real(0), imag(0) {}

        static constexpr T max_value = is_complex32 ? 0x7FFF'FFFF : is_complex16 ? 0x7FFF : 0x7F;
        static constexpr double max_value_f = is_complex32 ? 2147483647.0 : is_complex16 ? 32767.0 : 127.0;

        /* conversion operations */
        explicit constexpr operator complex32 () const {
            if      constexpr (is_complex32) return complex32 { real, imag };
            else if constexpr (is_complex16) return complex32 { real * 0x1'0000, imag * 0x1'0000 };
            return complex32 { real * 0x100'0000, imag * 0x100'0000 };
        }
        explicit constexpr operator complex16 () const {
            if      constexpr (is_complex32) return complex16 { real / 0x1'0000, imag / 0x1'0000 };
            else if constexpr (is_complex16) return complex16 { real, imag };
            return complex16 { real * 0x100, imag * 0x100 };
        }
        explicit constexpr operator complex8 () const {
            if      constexpr (is_complex32) return complex8 { real / 0x100'0000, imag / 0x100'0000 };
            else if constexpr (is_complex16) return complex8 { real / 0x100, imag / 0x100 };
            return complex8 { real, imag };
        }

        [[nodiscard]] constexpr float real_f() const { return static_cast<float> (real) / max_value_f; }
        [[nodiscard]] constexpr float imag_f() const { return static_cast<float> (imag) / max_value_f; }
        [[nodiscard]] constexpr Pair<float> to_float() const { return { real_f(), imag_f() }; }

        constexpr explicit operator bool () const { return real && imag; }

        /* arithmetic operations */
        template <class T2> constexpr enable_if_t<is_floating_point_v<T2>, complex> operator + (T2 val) const {
            return complex { real + from_float_(val), imag };
        }
        template <class T2> constexpr enable_if_t<is_floating_point_v<T2>, complex> operator - (T2 val) const {
            return complex { real - from_float_(val), imag };
        }
        template <class T2> constexpr enable_if_t<is_arithmetic_v<T2>, complex> operator * (T2 val) const {
            return complex { static_cast<T>(real * val), static_cast<T>(imag * val) };
        }
        template <class T2> constexpr enable_if_t<is_floating_point_v<T2>, complex> operator / (T2 val) const {
            return complex { real_f() / val, imag_f() / val };
        }
        template <class T2> constexpr enable_if_t<is_integral_v<T2>, complex> operator / (T2 val) const {
            return complex { real / val, imag / val };
        }

        template <class T2> constexpr auto operator + (const complex<T2>& other) const {
            auto b = static_cast<complex>(other);
            return complex { real + b.real, imag + b.imag };
        }
        template <class T2> constexpr auto operator - (const complex<T2>& other) const {
            auto b = static_cast<complex>(other);
            return complex { real - b.real, imag - b.imag };
        }
        template <class T2> constexpr auto operator * (const complex<T2>& other) const {
            auto [a, b] = pair16_(other);
            auto res = complex32 { a.real * b.real - a.imag * b.imag,
                                   a.imag * b.real + a.real * b.imag };
            return static_cast<complex>(res);
        }
        template <class T2> constexpr auto operator / (const complex<T2>& other) const {
            auto a = to_float();
            auto b = other.to_float();
            auto re = a.x * b.x + a.y * b.y;
            auto im = a.y * b.x - a.x * b.y;
            auto denum = b.x * b.x + b.y * b.y;
            return complex { re / denum, im / denum };
        }

        template <class T2> constexpr complex& operator += (const T2& val) { *this = *this + val; return *this; }
        template <class T2> constexpr complex& operator -= (const T2& val) { *this = *this - val; return *this; }
        template <class T2> constexpr complex& operator *= (const T2& val) { *this = *this * val; return *this; }
        template <class T2> constexpr complex& operator /= (const T2& val) { *this = *this / val; return *this; }
        template <class T2> constexpr complex& operator ^= (const T2& val) { *this = *this ^ val; return *this; }

        /* special operations */
        /// multiply this by conjugate of other
        template <class T2>
        [[nodiscard]] constexpr complex multiply_conjugate(const complex<T2>& other) const  {
            auto [a, b] = pair16_(other);
            auto res = complex32 { a.real * b.real + a.imag * b.imag,
                                   a.imag * b.real - a.real * b.imag };
            return static_cast<complex>(res);
        }
        template <class T2>
        [[nodiscard]] constexpr Pair<float> multiply_conjugate_f(const complex<T2>& other) const  {
            auto a = to_float();
            auto b = other.to_float();
            return { a.x * b.x + a.y * b.y,
                     a.y * b.x - a.x * b.y };
        }
        [[nodiscard]] constexpr int32_t magnitude_square() const {
            auto [re, im] = static_cast<complex8>(*this);
            return re * re + im * im;
        }
        [[nodiscard]] constexpr float magnitude_square_f() const {
            auto [re, im] = to_float();
            return re * re + im * im;
        }
        [[nodiscard]] constexpr float magnitude_f() const {
            return std::sqrt(magnitude_square_f());
        }
        [[nodiscard]] constexpr auto conjugate()   const { return complex { real, -imag}; }
        [[nodiscard]] constexpr auto operator - () const { return complex { -real, -imag }; }
        [[nodiscard]] float atan()                 const { return atan2f32(imag, real); }

        /// return multiply conjugate of other
        template <class T2>
        constexpr auto operator ^ (const complex<T2>& other) const { return multiply_conjugate(other); }

        /// return conjugate of this
        constexpr auto operator ~ () const { return conjugate(); }

        /// compare operation. compare the magnitude
        template <class T2>
        constexpr bool operator == (const complex<T2>& other) const {
            auto [a, b] = pair_smallest_(other);
            return a.magnitude_square() == b.magnitude_square();
        }
        template <class T2>
        constexpr bool operator > (const complex<T2>& other) const {
            auto [a, b] = pair_smallest_(other);
            return a.magnitude_square() > b.magnitude_square();
        }
        template <class T2>
        constexpr bool operator < (const complex<T2>& other) const {
            auto [a, b] = pair_smallest_(other);
            return a.magnitude_square() < b.magnitude_square();
        }

    private:
        template <class T2>
        [[nodiscard]] constexpr auto pair_smallest_(const complex<T2>& other) const {
            using smallest_complex = complex<smallest_size_t<T, T2>>;
            return Pair<smallest_complex> { static_cast<smallest_complex>(*this), static_cast<smallest_complex>(other) };
        }
        template <class T2>
        [[nodiscard]] constexpr auto pair16_(const complex<T2>& other) const {
            return Pair<complex16> { static_cast<complex16>(*this), static_cast<complex16>(other) };
        }

        static constexpr T from_float_(float val) { return static_cast<T>(max_value_f * val); }
    };

    /* is_complex type trait */
    template <typename T> struct is_complex {
        static const bool value = is_same_v<T, complex8> || is_same_v<T, complex16> || is_same_v<T, complex32>;
    };
    template <typename T> inline const bool is_complex_v = is_complex<T>::value;

    /* complex arithmatic operations */
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, complex<C>> operator + (T val, const complex<C>& c) {
        return c + val;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, complex<C>> operator - (T val, const complex<C>& c) {
        return -c + val;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, complex<C>> operator * (T val, const complex<C>& c) {
        return c * val;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, complex<C>> operator / (T val, const complex<C>& c) {
        return complex<C> { val } / c;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, complex<C>> operator ^ (T val, const complex<C>& c) {
        return complex<C> { val } ^ c;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, bool> operator == (T val, const complex<C>& c) {
        return complex<C> { val } == c;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, bool> operator > (T val, const complex<C>& c) {
        return complex<C> { val } > c;
    }
    template <class T, class C>
    constexpr enable_if_t<! is_complex_v<T>, bool> operator < (T val, const complex<C>& c) {
        return complex<C> { val } < c;
    }
}

namespace Project::etl::literals {
    constexpr auto operator ""_i(long double val) {
        return complex32 { 0.f, static_cast<float>(clamp(val, -1.0l, 1.0l)) };
    }
}

#endif //ETL_COMPLEX_H
