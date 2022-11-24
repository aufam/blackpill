#ifndef WTP_BLUEPILL_VECTOR_H
#define WTP_BLUEPILL_VECTOR_H

#include "main.h"

namespace Project::etl {

    template <class T>
    class Vector {
        T* buffer;
        size_t nItems;

    public:
        typedef T Type;

        template <class... Ts>
        Vector(Ts... items)
        : buffer(new T[sizeof...(items)] { items... })
        , nItems(sizeof...(items))
        {}
        Vector(T* buffer, size_t nItems) : buffer(buffer), nItems(nItems) {}
        constexpr Vector() : buffer(nullptr), nItems(0) {}

        ~Vector() {
            delete [] buffer;
            buffer = nullptr;
            nItems = 0;
        }

        [[nodiscard]] size_t len() const { return nItems; }
        void clear() { memset(buffer, 0, nItems); }

        T* data()   { return buffer; }
        T* begin()  { return buffer; }
        T* end()    { return buffer + nItems; }
        T& front()  { return buffer[0]; }
        T& back()   { return buffer[nItems - 1]; }

        const T* data()     const { return buffer; }
        const T* begin()    const { return buffer; }
        const T* end()      const { return buffer + nItems; }
        const T& front()    const { return buffer[0]; }
        const T& back()     const { return buffer[nItems - 1]; }

        T& operator [](size_t i) { return buffer[i]; }
        const T& operator [](size_t i) const { return buffer[i]; }

        Vector operator + (const Vector& other) const {
            auto temp = new T[nItems + other.nItems];
            memcpy(temp, buffer, nItems * sizeof (T));
            memcpy(temp + nItems, other.buffer, other.nItems * sizeof (T));
            return { temp, nItems + other.nItems };
        }
        Vector operator + (const T& other) const {
            auto temp = new T[nItems + 1];
            memcpy(temp, buffer, nItems * sizeof (T));
            memcpy(temp + nItems, &other, sizeof (T));
            return { temp, nItems + 1 };
        }

        void append(const Vector& other) {
            auto temp = *this + other;
            delete [] buffer;
            *this = temp;
        }
        void append(const T& other) {
            auto temp = *this + other;
            delete [] buffer;
            *this = temp;
        }

        Vector& operator += (const Vector& other) {
            append(other);
            return *this;
        }
        Vector& operator += (const T& other) {
            append(other);
            return *this;
        }

        void fill(const T& item) {
            for (T& it : *this) it = item;
        }

        template <class R>
        void fold(R& result, void (* fn)(R&, T&)) {
            for (T& item : *this) fn(result, item);
        }
        template <class R>
        void fold(R& result, void (* fn)(R&, const T&)) const {
            for (const T& item : *this) fn(result, item);
        }

        void foreach(void (* fn)(T&)) {
            for (T& item : *this) fn(item);
        }
        void foreach(void (* fn)(const T&)) const {
            for (const T& item : *this) fn(item);
        }

        bool any(bool (* check)(T&)) {
            for (T& item : *this) if (check(item)) return true;
            return false;
        }
        bool any(bool (* check)(const T&)) const {
            for (const T& item : *this) if (check(item)) return true;
            return false;
        }
        bool any(const T& check) const {
            for (const T& item : *this) if (item == check) return true;
            return false;
        }

        bool all(bool (*check)(T&)) {
            for (T& item : *this) if (!check(item)) return false;
            return true;
        }
        bool all(bool (*check)(const T&)) const {
            for (const T& item : *this) if (!check(item)) return false;
            return true;
        }
        bool all(const T& check) const {
            for (const T& item : *this) if (item != check) return false;
            return true;
        }
    };
}

#endif //WTP_BLUEPILL_VECTOR_H
