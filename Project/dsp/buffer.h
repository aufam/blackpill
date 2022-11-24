#ifndef PROJECT_BUFFER_H
#define PROJECT_BUFFER_H

#include "main.h"
#include "etl/array.h"
#include <cstring> // memcpy

namespace Project::DSP {

    /// Buffer class
    template <class T, size_t N>
    struct Buffer : etl::Array<T, N> {
    protected:
        static constexpr size_t min(size_t a, size_t b) { return a < b ? a : b; }
    };

    /// Circular buffer class
    template <class T, size_t N>
    struct BufferCirc : public Buffer<T, N> {
        size_t indexWrite;
        size_t indexRead;
        constexpr BufferCirc() : Buffer<T, N>(), indexWrite(0), indexRead(0) {}

        size_t numberOfWrite() {
            if (indexRead > indexWrite) return indexRead - indexWrite;
            return N + indexRead - indexWrite;
        }

        size_t numberOfRead() {
            if (indexRead <= indexWrite) return indexWrite - indexRead;
            return N + indexWrite - indexRead;
        }

        size_t write(const T *items, size_t nItems) {
            size_t n = numberOfWrite();
            nItems = this->min(n, nItems);
            if (nItems == 0) return 0;

            n = this->min((N - indexWrite), nItems);
            memcpy(this->buffer + indexWrite, items, n * sizeof(T));
            indexWrite += n;
            nItems -= n;

            if (nItems > 0) {
                memcpy(this->buffer, items + n, nItems * sizeof(T));
                indexWrite = nItems;
            }

            if (indexWrite >= N) indexWrite = 0;
            return n + nItems;
        }

        size_t read(T *items, size_t nItems) {
            size_t n = numberOfRead();
            nItems = this->min(n, nItems);
            if (nItems == 0) return 0;

            n = this->min((N - indexRead), nItems);
            memcpy(items, this->buffer + indexRead, n * sizeof(T));
            indexRead += n;
            nItems -= n;

            if (nItems > 0) {
                memcpy(items + n, this->buffer, nItems * sizeof(T));
                indexRead = nItems;
            }

            if (indexRead >= N) indexRead = 0;
            return n + nItems;
        }

        BufferCirc<T, N> &operator <<(const T& item) {
            write(&item, 1);
            return *this;
        }

        BufferCirc<T, N> &operator >>(T& item) {
            read(&item, 1);
            return *this;
        }
    };

    /// Double buffer class
    template <class T, size_t N>
    struct BufferDouble : public Buffer<T, N> {
        constexpr BufferDouble() : Buffer<T, N>() {}

        static constexpr size_t halfSize() { return N / 2; }
        [[nodiscard]] size_t halfLen() const { return N / 2; }

        T *half() { return this->buffer + halfSize(); }
        const T *half() const { return this->buffer + halfSize(); }
    };

} // namespace Project


#endif // PROJECT_BUFFER_H