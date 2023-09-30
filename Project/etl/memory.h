#ifndef ETL_ALIGNED_STORAGE_H
#define ETL_ALIGNED_STORAGE_H

#include "etl/utility_basic.h"

namespace Project::etl {
    
    /// provide exclusive ownership of a dynamically allocated object
    template <typename T>
    class unique_ptr {
        T* ptr;
    
    public:
        /// empty constructor
        unique_ptr() noexcept : ptr{nullptr} {}

        /// default constructor
        explicit unique_ptr(T* ptr) noexcept : ptr{ptr} {}

        /// disable copy constructor
        unique_ptr(const unique_ptr&) = delete;

        /// disable copy assignment
        unique_ptr& operator=(const unique_ptr&) = delete;

        /// move constructor
        unique_ptr(unique_ptr&& other) noexcept : ptr{other.release()} {}

        /// move assigment
        unique_ptr& operator=(unique_ptr&& other) noexcept { if (this != &other) reset(other.release()); return *this;}
        
        /// default destructor
        ~unique_ptr() noexcept { if (ptr) delete ptr; }

        /// return true if ptr is not null
        explicit operator bool() const noexcept { return static_cast<bool>(ptr); }

        /// arrow operator
        T* operator->() noexcept { return ptr; }
        const T* operator->() const noexcept { return ptr; }

        /// dereference operator
        T& operator*() noexcept { return *ptr; }
        const T& operator*() const noexcept { return *ptr; }

        /// get the pointer
        T* get() noexcept { return ptr; }
        const T* get() const noexcept { return ptr; }

        /// return its value if pointer is not null or return other
        constexpr const T& get_value_or(const T& other) const { return ptr ? *ptr : other; }
        constexpr T& get_value_or(T& other) { return ptr ? *ptr : other; }
        
        /// set new object pointer and delete the old one
        void reset(T* other = nullptr) noexcept { T* old = etl::exchange(ptr, other); if (old) delete old; }

        /// release the ownership, the caller must handle the pointer
        [[nodiscard]]
        T* release() noexcept { return etl::exchange(ptr, nullptr); }
    };

    /// create a unique pointer object
    template <typename T, typename... Args> auto
    make_unique(Args&&... args) { return unique_ptr<T>(new T(etl::forward<Args>(args)...)); }

    /// provide shared ownership of a dynamically allocated object
    template <typename T>
    class shared_ptr {
        T* ptr;
        size_t* cnt;

    public:
        /// empty constructor
        shared_ptr() noexcept : ptr{nullptr}, cnt{new size_t(0)} {}

        /// default constructor
        explicit shared_ptr(T* ptr) noexcept : ptr{ptr}, cnt{new size_t(ptr ? 1 : 0)} {}
        
        /// copy constructor
        shared_ptr(const shared_ptr& other) noexcept : ptr{other.ptr}, cnt{other.cnt} { 
            inc_(); 
        }

        /// copy assignment
        shared_ptr& operator=(const shared_ptr& other) noexcept {
            if (this == &other) return *this;
            delete_();
            ptr = other.ptr;
            cnt = other.cnt;
            inc_();
            return *this;
        }

        /// move constructor
        shared_ptr(shared_ptr&& other) noexcept 
            : ptr{etl::exchange(other.ptr, nullptr)}
            , cnt{etl::exchange(other.cnt, nullptr)} {}

        /// move assignment
        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this == &other) return *this;
            delete_();
            ptr = etl::exchange(other.ptr, nullptr);
            cnt = etl::exchange(other.cnt, nullptr);
            return *this;
        }

        /// default constructor
        ~shared_ptr() { delete_(); }

        /// return true if the shared pointer is not null
        explicit operator bool() noexcept { return static_cast<bool>(ptr); }
        
        /// arrow operator
        T* operator->() noexcept { return ptr; }
        const T* operator->() const noexcept { return ptr; }

        /// dereference operator
        T& operator*() noexcept { return *ptr; }
        const T& operator*() const noexcept { return *ptr; }

        /// get the pointer
        T* get() noexcept { return ptr; }
        const T* get() const noexcept { return ptr; }

        /// return its value if pointer is not null or return other
        constexpr const T& get_value_or(const T& other) const { return ptr ? *ptr : other; }
        constexpr T& get_value_or(T& other) { return ptr ? *ptr : other; }

        /// set new object pointer and delete the old one
        void reset(T* other = nullptr) noexcept { 
            delete_();
            ptr = other;
            cnt = new size_t(1);
        }

        /// release the ownership
        /// @return released pointer and reference count
        /// @note if the reference count is zero, the caller must handle the released pointer
        [[nodiscard]]
        auto release() noexcept { 
            struct Released { T* ptr; size_t count; };
            if (cnt && --*cnt == 0) delete cnt;
            size_t count_ = count();
            cnt = nullptr;
            return Released { etl::exchange(ptr, nullptr), count_ }; 
        }

        /// returns the number of shared pointers that share ownership of the managed object
        size_t count() const noexcept { return cnt ? *cnt : 0; }

    private:
        /// delete ptr helper
        void delete_() {
            if (ptr && cnt && --*cnt == 0) {
                delete ptr;
                ptr = nullptr;
                delete cnt;
                cnt = nullptr;
            } 
        }
        
        /// inc helper
        void inc_() { 
            if (ptr == nullptr) return;
            if (cnt) ++*cnt; 
            else cnt = new size_t(1);
        }

    };

    /// create shared pointer object
    template <typename T, typename... Args> auto
    make_shared(Args&&... args) { return shared_ptr<T>(new T(etl::forward<Args>(args)...)); }
}

#endif // ETL_ALIGNED_STORAGE_H