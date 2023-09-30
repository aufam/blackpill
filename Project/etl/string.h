#ifndef ETL_STRING_H
#define ETL_STRING_H

#include <cstring> // strlen, strcpy, memset
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

#ifndef ETL_STRING_DEFAULT_SIZE
#define ETL_STRING_DEFAULT_SIZE 64
#endif

#ifndef ETL_SHORT_STRING_DEFAULT_SIZE
#define ETL_SHORT_STRING_DEFAULT_SIZE 16
#endif

#include "etl/algorithm.h"

namespace Project::etl {
    template <size_t N> class SplitString;

    /// simple string class with c-style formatter
    template <size_t N = ETL_STRING_DEFAULT_SIZE>
    class String {
        char str[N];

    public:
        static_assert(N > 0, "String size can't be zero");

        typedef char value_type;
        typedef char* iterator;
        typedef const char* const_iterator;
        typedef char& reference;
        typedef const char& const_reference;

        /// empty constructor
        constexpr String() : str{} {}

        /// construct from array char with different size 
        template <size_t M>
        constexpr String(const char (&text)[M]) : str{} { // NOLINT
            etl::copy(text, text + etl::min(N, M), begin());
            str[N - 1] = '\0';
        }

        /// C-style format constructor
        template <typename Arg, typename... Args>
        String(const char* fmt, Arg arg, Args... args) : str{} {
            operator()(fmt, arg, args...);
        }

        /// copy constructor
        template <size_t M>
        constexpr String(const String<M>& other) : str{} { // NOLINT
            *this = other;
        }

        /// copy assignment
        template <size_t M>
        constexpr String& operator=(const String<M>& other) {
            if constexpr (N == M) if (this == &other) return *this;
            auto n = etl::min(N, M);
            etl::copy(other.begin(), other.begin() + n, str);
            str[N - 1] = '\0';
            return *this;
        }

        /// move constructor
        template <size_t M>
        constexpr String(String<M>&& other) noexcept : str{} { // NOLINT
            *this = etl::move(other);
        }

        /// move assignment
        template <size_t M>
        constexpr String& operator=(String<M>&& other) noexcept {
            if constexpr (N == M) if (this == &other) return *this;
            auto n = etl::min(N, M);
            etl::move(other.begin(), other.begin() + n, str);
            str[N - 1] = '\0';
            other.clear();
            return *this;
        }

        /// size in bytes
        static constexpr size_t size() { return N; }

        /// string length, maximum is N - 1
        [[nodiscard]] constexpr size_t len() const { return etl::find(str, str + N - 1, '\0') - str; }

        /// remaining space
        [[nodiscard]] constexpr size_t rem() const { return N - 1 - len(); }

        /// set all characters to 0
        constexpr void clear() { etl::fill(str, '\0'); }

        constexpr iterator data()   { return str; }
        constexpr iterator begin()  { return str; }
        constexpr iterator end()    { return str + len(); }
        constexpr reference front() { return str[0]; }
        constexpr reference back()  { auto l = len(); return str[l ? l - 1 : 0]; }

        constexpr const_iterator data()   const { return str; }
        constexpr const_iterator begin()  const { return str; }
        constexpr const_iterator end()    const { return str + len(); }
        constexpr const_reference front() const { return str[0]; }
        constexpr const_reference back()  const { auto l = len(); return str[l ? l - 1 : 0]; }

        constexpr Iter<iterator> iter() { return Iter(begin(), end(), 1); }
        constexpr Iter<const_iterator> iter() const { return Iter(begin(), end(), 1); }

        constexpr Iter<iterator> reversed() { return Iter(end() - 1, begin() - 1, -1); }
        constexpr Iter<const_iterator> reversed() const { return Iter(end() - 1, begin() - 1, -1); }

        constexpr reference operator[](int i) {
            if (i < 0) i = len() + i;
            return str[i];
        }
        constexpr const_reference operator[](int i) const {
            if (i < 0) i = len() + i;
            return str[i];
        }

        /// return true if first character is not '\0'
        constexpr explicit operator bool() const { return str[0] != '\0'; }

        /// assign from other const char*
        constexpr String& operator=(const char* other) {
            size_t i = 0;
            for (; i < N - 1 && other[i] != '\0'; i++) str[i] = other[i];
            str[i] = '\0';
            return *this;
        }

        /// assign from char
        constexpr String& operator=(char ch) {
            if (N == 1) return *this;
            str[0] = ch;
            str[1] = '\0';
            return *this;
        }

        /// append operator from other string
        template <size_t M>
        constexpr String& operator+=(const String<M>& other)  {
            auto offset = other.data() + etl::min(rem(), other.len());
            auto last = etl::copy(other.data(), offset, end());
            *last = '\0';
            return *this;
        }

        /// append operator from other const char*
        constexpr String& operator+=(const char* other) {
            size_t i = 0, l = len(), r = rem();
            for (; i < r && other[i] != '\0'; i++) str[l + i] = other[i];
            str[l + i] = '\0';
            return *this;
        }

        /// append operator from char
        constexpr String &operator+=(char ch) {
            if (rem() == 0) return *this;
            auto i = len();
            str[i] = ch;
            str[i + 1] = '\0';
            return *this;
        }

        template <size_t M>
        constexpr String &operator<<(const String<M>& other)  { *this += other; return *this; }

        constexpr String &operator<<(const char* other)       { *this += other; return *this; }
        
        constexpr String &operator<<(char ch)                 { *this += ch; return *this; }

        /// C-style formatter
        char* operator()(const char* fmt, ...) {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, N, fmt, vl);
            va_end(vl);
            str[N - 1] = '\0';
            return str;
        }

        /* compare functions */
        template <size_t M> int
        compare(const String<M>& other)           const { return strncmp(str, other.data(), N); }
        
        template <size_t M> int
        compare(const String<M>& other, size_t n) const { return strncmp(str, other.data(), n); }

        int compare(const char* other, size_t n)  const { return strncmp(str, other, n); }
        
        int compare(const char* other)            const { return strncmp(str, other, N); }

        /* compare operators */
        template <size_t M> bool
        operator==(const String<M>& other) const { return compare(other) == 0; }
        
        template <size_t M> bool
        operator>(const String<M>& other)  const { return compare(other) > 0; }
        
        template <size_t M> bool
        operator<(const String<M>& other)  const { return compare(other) < 0; }

        bool operator==(const char* other) const { return compare(other) == 0; }
        
        bool operator>(const char* other)  const { return compare(other) > 0; }
        
        bool operator<(const char* other)  const { return compare(other) < 0; }

        template <size_t M> bool
        operator!=(const String<M>& other) const { return !operator==(other); }

        bool operator!=(const char* other) const { return !operator==(other); }

        /* find a substring inside this string */
        size_t find(const char* substring, size_t n) const {
            size_t i = 0;
            for (auto &ch : *this) {
                if (strncmp(substring, &ch, n) == 0) break;
                i++;
            }
            return i;
        }

        size_t find(const char* substring) const { return find(substring, strlen(substring)); }

        template <size_t M>
        size_t find(const String<M>& substring) const { return find(substring, substring.len()); }

        /* check if a substring is inside this string */
        bool isContaining(const char* substring, size_t n) { return find(substring, n) < len(); }

        bool isContaining(const char* substring) { return find(substring) < len(); }

        template <size_t M>
        bool isContaining(const String<M>& substring) { return find(substring) < len(); }

        template <size_t M = ETL_SHORT_STRING_DEFAULT_SIZE>
        auto split(const char* separator = " ") { return SplitString<M>(str, separator); }
    };

    /// create string from string literal, size can be implicitly or explicitly specified
    template <size_t n = 0, size_t M, size_t N = (n > 0 ? n : M)> constexpr auto
    string(const char (&text)[M]) { return String<N> { text }; }

    /// create C-style formatted string, default size is ETL_STRING_DEFAULT_SIZE
    template <size_t N = ETL_STRING_DEFAULT_SIZE, typename Arg, typename... Args> auto
    string(const char* fmt, Arg arg, Args... args) { return String<N> { fmt, arg, args... }; }

    /// create empty string, default size is ETL_STRING_DEFAULT_SIZE 
    template <size_t N = ETL_STRING_DEFAULT_SIZE> constexpr auto 
    string() { return String<N>{}; }

    /// create short string, size is ETL_SHORT_STRING_DEFAULT_SIZE 
    template <size_t N> constexpr auto 
    short_string(const char (&text)[N]) { return etl::string<ETL_SHORT_STRING_DEFAULT_SIZE>(text); }

    /// create C-style formatted string, size is ETL_SHORT_STRING_DEFAULT_SIZE
    template <typename Arg, typename... Args> auto
    short_string(const char* fmt, Arg arg, Args... args) { return etl::string<ETL_SHORT_STRING_DEFAULT_SIZE>(fmt, arg, args...); }

    /// create empty string, size is ETL_SHORT_STRING_DEFAULT_SIZE 
    constexpr auto 
    short_string() { return etl::string<ETL_SHORT_STRING_DEFAULT_SIZE>(); }

    /// create string from 2 strings, size is s1.size() + s2.size() - 1
    /// @warning string buffer might be unnecessarily large
    template <size_t N, size_t M> constexpr auto
    operator+(const String<N>& s1, const String<M>& s2) {
        String<N + M -1> res = s1;
        res += s2;
        return res;
    }

    /// overload
    template <size_t N> constexpr bool
    operator==(const char* x, const String<N>& y) { return y == x; }

    /// overload
    template <size_t N> constexpr bool
    operator!=(const char* x, const String<N>& y) { return y != x; }

    /// cast reference from any pointer
    template <size_t N, typename T> constexpr auto&
    string_cast(T* text) { return *reinterpret_cast<conditional_t<is_const_v<T>, const String<N>*, String<N>*>>(text); }

    /// cast reference from any type
    template <typename T> constexpr auto&
    string_cast(T& text) { return etl::string_cast<sizeof(T)>(&text); }

    /// swap specialization
    template <size_t N, size_t M> constexpr void
    swap(String<N>& s1, String<M>& s2) {
        if constexpr (etl::min(N, M) == N) {
            String<N> temp(etl::move(s1));
            s1 = etl::move(s2);
            s2 = etl::move(temp);
        }
        else {
            String<M> temp(etl::move(s2));
            s2 = etl::move(s1);
            s1 = etl::move(temp);
        }
    }

    /// swap_element specialization
    template <size_t N, size_t M> constexpr void
    swap_element(String<N>& s1, String<M>& s2) { etl::swap(s1, s2); }

    /// sum specialization
    template <size_t N> constexpr auto
    sum_element(const String<N>&) = delete;

    /// type traits
    template <size_t N> struct is_string<String<N>> : true_type {};
    template <size_t N> struct is_string<const String<N>> : true_type {};
    template <size_t N> struct is_string<volatile String<N>> : true_type {};
    template <size_t N> struct is_string<const volatile String<N>> : true_type {};

    template <size_t N> struct is_array<String<N>> : true_type {};
    template <size_t N> struct is_array<const String<N>> : true_type {};
    template <size_t N> struct is_array<volatile String<N>> : true_type {};
    template <size_t N> struct is_array<const volatile String<N>> : true_type {};

    template <size_t N> struct remove_extent<String<N>> { typedef char type; };
    template <size_t N> struct remove_extent<const String<N>> { typedef char type; };
    template <size_t N> struct remove_extent<volatile String<N>> { typedef char type; };
    template <size_t N> struct remove_extent<const volatile String<N>> { typedef char type; };

    /// simple split string using strtok
    /// @tparam N string buf size, default = ETL_STRING_DEFAULT_SIZE
    /// @tparam M argv buf size, default = ETL_SHORT_STRING_DEFAULT_SIZE
    template <size_t N = ETL_SHORT_STRING_DEFAULT_SIZE>
    class SplitString {
        mutable size_t argc = 0;    ///< number of arguments
        mutable char* argv[N] = {}; ///< array of argument values

    public: 
        typedef char* value_type;
        typedef char** iterator;
        typedef const char** const_iterator;
        typedef char* reference;
        typedef const char* const_reference;
        
        /// construct from char*
        explicit SplitString(char* text, const char* separator = " ") {
            for (; argc < N; argc++) {
                argv[argc] = strtok(argc == 0 ? text : nullptr, separator);
                if (argv[argc] == nullptr) break;
            }
        }

        /// construct from String
        template <size_t M = ETL_SHORT_STRING_DEFAULT_SIZE>
        explicit SplitString(String<M>& text, const char* separator = " ")
        : SplitString(text.data(), separator) {}

        size_t len() const { return argc; }

        iterator begin()  { return &argv[0]; }
        iterator end()    { return &argv[argc]; }
        reference front() { return argv[0]; }
        reference back()  { return argv[argc - 1]; }

        const_iterator begin()  const { return &argv[0]; }
        const_iterator end()    const { return &argv[argc]; }
        const_reference front() const { return argv[0]; }
        const_reference back()  const { return argv[argc - 1]; }

        reference operator[](size_t index) { return index < argc ? argv[index] : nullptr; }
        const_reference operator[](size_t index) const { return index < argc ? argv[index] : nullptr; }
    };
}

namespace Project::etl::literals {

    constexpr auto operator ""s(const char* text, size_t n) {
        String<ETL_SHORT_STRING_DEFAULT_SIZE> s;
        copy(text, text + n, s.data());
        s[ETL_SHORT_STRING_DEFAULT_SIZE - 1] = '\0';
        return s;
    }
}

#endif //ETL_STRING_H
