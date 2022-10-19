#ifndef PROJECT_FSTRING_H
#define PROJECT_FSTRING_H

#include "main.h"
#include <cstring> // strlen, strcpy
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

namespace Project {

    template <size_t BUFFER_SIZE>
    struct String {
        char str[BUFFER_SIZE];
        constexpr String() : str{} {}
        explicit String(const char *str) : str{} { strcpy(this->str, str); }

        size_t len()   { return strlen(str); }
        char *data()   { return str; }
        char *begin()  { return str; }
        char *end()    { return str + len(); }

        template <size_t N>
        String &operator=(const String<N> &other)   { strcpy(str, other.str); return *this; };
        String &operator=(const char *other)        { strcpy(str, other); return *this; };

        template <size_t N>
        String &operator+=(const String<N> &other)  { strcpy(end(), other.str); return *this; };
        String &operator+=(const char *other)       { strcpy(end(), other); return *this; };
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
