#ifndef ETL_ANY_H
#define ETL_ANY_H

#include "etl/utility_basic.h"

namespace Project::etl {

    /// provide a dynamic storage for storing and manipulating value of any type
    /// @note if the type has custom destructor, you have to manually delete it by invoking detach<T>() 
    /// before destructing or reassigning this object
    class Any {
        uint8_t* ptr;
        size_t n;

    public:
        /// empty constructor
        constexpr Any() : ptr(nullptr), n(0) {}

        /// copy constructor
        Any(const Any& other) : ptr(new uint8_t[other.n]), n(other.n) {
            if (ptr == nullptr) {
                n = 0;
                return;
            }

            for (size_t i = 0; i < n; i++)
                ptr[i] = other.ptr[i];
        }

        /// copy assignment
        Any& operator=(const Any& other) {
            if (&other == this) return *this;
            detach();
            
            ptr = new uint8_t[other.n];
            n = other.n;
            if (ptr == nullptr) {
                n = 0;
                return *this;
            }

            for (size_t i = 0; i < n; i++)
                ptr[i] = other.ptr[i];
            return *this;
        }

        /// move constructor
        Any(Any&& other) noexcept : ptr(etl::exchange(other.ptr, nullptr)), n(etl::exchange(other.n, 0)) {}

        /// move assignment
        Any& operator=(Any&& other) noexcept {
            if (&other == this) return *this;
            detach();

            ptr = etl::exchange(other.ptr, nullptr);
            n = etl::exchange(other.n, 0);
            return *this;
        }

        /// construct from any type
        /// @note disable if value type is Any& or const T&&
        template <typename T, typename = disable_if_t<is_same_v<T, Any&> || is_const_v<T>>>
        Any(T&& value) : ptr(nullptr), n(0) {
            using U = decay_t<T>;
            if constexpr (!is_same_v<U, None>) {
                ptr = new uint8_t[sizeof(U)];
                if (ptr == nullptr) 
                    return;
                
                new(ptr) U(etl::forward<T>(value));
                n = sizeof(U);
            }
        }

        /// assign from any type
        /// @note disable if value type is Any& or const T&&
        template <typename T, typename = disable_if_t<is_same_v<T, Any&> || is_const_v<T>>>
        Any& operator=(T&& value) {
            Any other = etl::forward<T>(value);
            *this = etl::move(other);
            return *this;
        }

        ~Any() { detach(); }

        size_t size() const { return n; }

        /// invoke destructor T and reset
        template <typename T = void, typename = disable_if_t<is_same_v<T, Any> || is_const_v<T>>>
        void detach() {
            if (ptr == nullptr) return;
            
            if constexpr (is_compound_v<T> && !is_pointer_v<T>)
                reinterpret_cast<T*>(ptr)->~T();
            
            delete[] ptr;
            ptr = nullptr;
            n = 0;
        }

        /// cast to any type
        /// @note the type has to be the last type this object is assigned to
        template <typename T> const T& as() const { return *reinterpret_cast<const T*>(ptr); }
        template <typename T> T& as() { return *reinterpret_cast<T*>(ptr); }

        /// get raw pointer
        const void* get() const { return ptr; }
        void* get() { return ptr; }

        template <typename T> const T& get_value_or(const T& other) const { return ptr ? as<T>() : other; }
        template <typename T> T& get_value_or(T& other) const { return ptr ? as<T>() : other; }

        explicit operator bool() const { return bool(ptr); }
    };

    /// create empty any object
    inline auto any() { return etl::Any(); }

    /// create any object of a given value
    template <typename T> auto
    any(T&& value) { return etl::Any(etl::forward<T>(value)); }
}

#endif // ETL_ANY_H