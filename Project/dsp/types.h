#ifndef PROJECT_DSP_TYPES_H
#define PROJECT_DSP_TYPES_H

#include "main.h"
#include <cmath>

constexpr float operator ""_pi(long double val)         { return float(val) * float(M_PI); }
constexpr float operator ""_pi(unsigned long long val)  { return float(val) * float(M_PI); }

constexpr size_t operator ""_k(unsigned long long val)  { return val * 1000; }
constexpr size_t operator ""_k(long double val)         { return size_t(val * 1000); }

constexpr size_t operator ""_M(unsigned long long val)  { return val * 1000_k; }
constexpr size_t operator ""_M(long double val)         { return size_t(val * 1000_k); }

constexpr size_t operator ""_G(unsigned long long val)  { return val * 1000_M; }
constexpr size_t operator ""_G(long double val)         { return size_t(val * 1000_M); }

namespace Project::DSP {

    /// amplitude type normalized to 8 bit. float value is -1.0 (128u) to 1.0 (127u)
    struct amplitude8_t { // float val is -1.0 to 1.0
        union { uint8_t u; int8_t i; };
        static const float lut_f8[256];

        constexpr explicit amplitude8_t(float val = 0)   : u(int(val * 127)) {}
        constexpr explicit amplitude8_t(double val = 0)  : u(int(val * 127)) {}
        constexpr explicit amplitude8_t(uint8_t val = 0) : u(val) {}
        constexpr explicit amplitude8_t(int8_t val = 0)  : i(val) {}
        constexpr explicit amplitude8_t(int16_t val = 0) : i(int8_t(val >> 8)) {}
        constexpr explicit amplitude8_t(int32_t val = 0) : i(int8_t(val >> 24)) {}

        explicit operator float() const { return lut_f8[u]; }
    };

    /// angle type normalized to 32 bit. float value is -pi to pi
    struct angle32_t {
        int32_t val;
        static const int8_t lut_sin[256];

        static constexpr float fold(float val) {
            return val - std::floor(val / 2_pi + .5f) * 2_pi;
        }
        static constexpr int32_t floatToFix(float f) {
            return (int32_t) (fold(f) * float(0xFFFFFFFF) / 2_pi);
        }
        static constexpr float fixToFloat(int32_t i) {
            return (float) i * (2_pi / float(0xFFFFFFFF));
        }

        constexpr explicit angle32_t(float val)     : val(floatToFix(val)) {}
        constexpr explicit angle32_t(int32_t val)   : val(val) {}

        constexpr explicit operator float() const { return fixToFloat(val); }

        constexpr angle32_t operator +(const angle32_t &other) const { return angle32_t{ val + other.val }; }
        constexpr angle32_t operator -(const angle32_t &other) const { return angle32_t{ val - other.val }; }

        constexpr angle32_t operator +(const int32_t &other) const { return angle32_t{ val + other }; }
        constexpr angle32_t operator -(const int32_t &other) const { return angle32_t{ val - other }; }

        constexpr angle32_t operator +(const float &other) const { return angle32_t{val + floatToFix(other) }; }
        constexpr angle32_t operator -(const float &other) const { return angle32_t{val - floatToFix(other) }; }

        angle32_t &operator +=(const angle32_t &other) {
            val = val + other.val;
            return *this;
        }
        angle32_t &operator +=(const int32_t &other) {
            val = val + other;
            return *this;
        }
        angle32_t &operator +=(const float &other) {
            val = val + floatToFix(other);
            return *this;
        }

        [[nodiscard]] amplitude8_t sin() const {
            auto index = (val >> 24) & 0xFF;
            return amplitude8_t{ lut_sin[index] };
        }
        [[nodiscard]] amplitude8_t cos() const {
            auto index = ((val >> 24) + 64) & 0xFF;
            return amplitude8_t{ lut_sin[index] };
        }
    };

    /// complex type
    template <class T>
    struct complex {
        T real, imag;
        constexpr complex(amplitude8_t real, amplitude8_t imag) : real(real.i), imag(imag.i) {}
        constexpr explicit complex(T real = 0, T imag = 0) : real(real), imag(imag) {}

        [[nodiscard]] float getReal() const { return float(amplitude8_t(real)); }
        [[nodiscard]] float getImag() const { return float(amplitude8_t(imag)); }

        [[nodiscard]] float atanf() const     { return atan2f32(imag, real); }
        [[nodiscard]] angle32_t atani() const { return { this->atanf() }; }

        [[nodiscard]] complex<int32_t> multiplyConjugate(const complex<T> &other) const {
            return {
                    real * other.real + imag * other.imag,
                    imag * other.real - real * other.imag
            };
        }

        complex<int32_t> operator *(const complex<T> &other) const {
            return {
                    real * other.real - imag * other.imag,
                    imag * other.real + real * other.imag
            };
        }

    };

    using complex8_t  = complex<int8_t>;
    using complex16_t = complex<int16_t>;
    using complex32_t = complex<int32_t>;
}

#endif //PROJECT_DSP_TYPES_H
