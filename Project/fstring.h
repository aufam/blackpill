#ifndef PROJECT_FSTRING_H
#define PROJECT_FSTRING_H

#include "main.h"
#include <cstring> // strlen, strcpy
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

namespace Project {

    /// simple string class
    template <size_t BUFFER_SIZE = 64>
    struct String {
        char str[BUFFER_SIZE];
        constexpr String() : str{} {}
        explicit String(const char* fmt, ...) : str{} {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, BUFFER_SIZE, fmt, vl);
            va_end(vl);
        }

        size_t size()  { return BUFFER_SIZE; }
        size_t len()   { return strlen(str); }
        size_t rem()   { return size() - len(); } ///< remaining space
        char *data()   { return str; }
        char *begin()  { return str; }
        char *end()    { return str + len(); }

        template <size_t N>
        String &operator=(const String<N> &other)   { strncpy(str, other.str, size() - 1); return *this; };
        String &operator=(const char *other)        { strncpy(str, other, size() - 1); return *this; };

        template <size_t N>
        String &operator+=(const String<N> &other)  { strncpy(end(), other.str, rem() - 1); return *this; };
        String &operator+=(const char *other)       { strncpy(end(), other, rem() - 1); return *this; };
        String &operator+=(char ch) {
            auto index = len();
            str[index++] = ch;
            str[index] = '\0';
            return *this;
        }

        char *operator ()(const char* fmt, ...) {
            va_list vl;
            va_start(vl, fmt);
            vsnprintf(str, BUFFER_SIZE, fmt, vl);
            va_end(vl);
            return str;
        }
    };

}

#endif //PROJECT_FSTRING_H
