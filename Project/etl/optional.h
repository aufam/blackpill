#ifndef ETL_OPTIONAL_H
#define ETL_OPTIONAL_H

#include "etl/utility_basic.h"

namespace Project::etl::detail {
    /// optional helper to check all arguments are not null
    template <typename Arg, typename... Args> constexpr bool
    optional_check_arguments(Arg arg, Args... args) {
        if constexpr (sizeof...(Args) == 0)
            return arg;
        else 
            return arg && optional_check_arguments(args...);
    }
}

namespace Project::etl {

    /// provide a way to represent an optional value that may or may not be present
    template <typename T>
    class Optional {
        bool valid;
        T value;

    public:
        typedef T type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T&& rval_reference;

        /// empty constructor
        constexpr Optional() : valid(false), value() {}

        /// copy construct from original type
        constexpr explicit Optional(const_reference value) : Optional(bool(etl::addressof(value)), value) {}

        /// move construct from original type
        constexpr explicit Optional(rval_reference value) : Optional(bool(etl::addressof(value)), etl::move(value)) {}

        /// copy construct from another Optional<T>
        Optional(const Optional&) = default;

        /// move construct from another Optional<T>
        Optional(Optional&&) noexcept = default;

        /// copy assign from original type
        constexpr Optional& operator=(const_reference other) { return assign(bool(etl::addressof(other)), other); }

        /// move assign from original type
        constexpr Optional& operator=(rval_reference other) { return assign(bool(etl::addressof(other)), etl::move(other)); }

        /// copy assign from another Optional<T>
        Optional& operator=(const Optional&) = default;

        /// move assign from another Optional<T>
        Optional& operator=(Optional&&) noexcept = default;

        /// move/copy assign from another convertible type U
        template <typename U>
        constexpr Optional& operator=(U&& other) { return assign(bool(etl::addressof(other)), T(etl::forward<U>(other))); }

        /// assign none type
        constexpr Optional& operator=(None) { valid = false; return *this; }
        
        /// check if the value is valid
        constexpr explicit operator bool() const { return valid; }

        /// arrow operator
        constexpr const_pointer operator->() const { return &value; }
        constexpr pointer operator->() { return &value; }

        /// dereference operator
        constexpr const_reference operator*() const { return valid ? value : *static_cast<const_pointer>(nullptr); }
        constexpr reference operator*() { return valid ? value : *static_cast<pointer>(nullptr); }

        /// get pointer
        constexpr const_pointer get() const { return &value; }
        constexpr pointer get() { return &value; }

        /// return value if valid or return other
        constexpr const_reference get_value_or(const_reference other) const { return valid ? value : other; }
        constexpr reference get_value_or(reference other) { return valid ? value : other; }

    private:

        /// default copy constructor helper
        constexpr Optional(bool cond, const_reference value) : valid(cond), value(cond ? value : type()) {}

        /// default move constructor helper
        constexpr Optional(bool cond, rval_reference value) : valid(cond), value(cond ? etl::move(value) : type()) {}

        /// copy assign helper
        constexpr Optional& assign(bool cond, const_reference other) {
            valid = cond;
            if (valid) value = other;
            return *this;
        }

        /// move assign helper
        constexpr Optional& assign(bool cond, rval_reference other) {
            valid = cond;
            if (valid) value = etl::move(other);
            return *this;
        }
    };

    /// create empty optional object
    template <typename T> constexpr auto
    optional() { return Optional<T>(); }

    /// create optional with variadic template function, the type can be explicitly or implicitly specified
    template < typename T = void, 
               typename Arg, typename... Args, 
               typename R = conditional_t<is_void_v<T>, remove_const_volatile_ref_t<Arg>, T> > constexpr auto
    optional(Arg&& arg, Args&&... args) { 
        using Arg_ = remove_const_volatile_ref_t<Arg>;

        if constexpr (sizeof...(Args) == 0 && is_same_v<None, Arg_>)
            return Optional<R>();

        // Arg is R
        else if constexpr (sizeof...(Args) == 0 && is_same_v<R, Arg_>)
            return Optional<R>(etl::forward<Arg_>(arg));

        // Args are the arguments of constructor R
        else {
            // each argument address can't be null
            if (detail::optional_check_arguments(&arg, &args...))
                return Optional<R>(R(etl::forward<Arg_>(arg), etl::forward<remove_const_volatile_ref_t<Args>>(args)...));
            else
                return Optional<R>();
        }
    }
}

#endif // ETL_OPTIONAL_H