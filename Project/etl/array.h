#ifndef ETL_ARRAY_H
#define ETL_ARRAY_H

#include "main.h"

namespace Project::etl {

    template <class T, size_t N>
    struct Array {
        typedef T Type;
        T buffer[N];

        static constexpr size_t size() { return N; }
        [[nodiscard]] size_t len() const { return N; }
        void clear() { memset(buffer, 0, N); }

        T* data()   { return buffer; }
        T* begin()  { return buffer; }
        T* end()    { return buffer + N; }
        T& front()  { return buffer[0]; }
        T& back()   { return buffer[N - 1]; }

        const T* data()     const { return buffer; }
        const T* begin()    const { return buffer; }
        const T* end()      const { return buffer + N; }
        const T& front()    const { return buffer[0]; }
        const T& back()     const { return buffer[N - 1]; }

        T& operator [](size_t i) { return buffer[i]; }
        const T& operator [](size_t i) const { return buffer[i]; }
        
        void fill(const T& item, size_t n = N) {
            for (size_t i = 0; i < n; i++) buffer[i] = item; 
        }
        
        template <class R>
        void fold(R& result, void (* fn)(R&, T&), size_t n = N) {
            for (size_t i = 0; i < n; i++) fn(result, buffer[i]);
        }
        template <class R>
        void fold(R& result, void (* fn)(R&, const T&), size_t n = N) const {
            for (size_t i = 0; i < n; i++) fn(result, buffer[i]);
        }
        
        void foreach(void (* fn)(T&), size_t n = N) { 
            for (size_t i = 0; i < n; i++) fn(buffer[i]); 
        }
        void foreach(void (* fn)(const T&), size_t n = N) const { 
            for (size_t i = 0; i < n; i++) fn(buffer[i]); 
        }
        
        bool any(bool (* check)(T&)) { 
            for (T& item : buffer) if (check(item)) return true;
            return false; 
        }
        bool any(bool (* check)(const T&)) const { 
            for (const T& item : buffer) if (check(item)) return true;
            return false; 
        }
        bool any(const T& check) const { 
            for (const T& item : buffer) if (item == check) return true;
            return false; 
        }
        
        bool all(bool (*check)(T&)) { 
            for (T& item : buffer) if (!check(item)) return false;
            return true; 
        }
        bool all(bool (*check)(const T&)) const { 
            for (const T& item : buffer) if (!check(item)) return false;
            return true; 
        }
        bool all(const T& check) const { 
            for (const T& item : buffer) if (item != check) return false;
            return true; 
        }
    };

}

#endif //ETL_ARRAY_H
