#ifndef ETL_VECTOR_H
#define ETL_VECTOR_H

#include "etl/algorithm.h"

namespace Project::etl {

    /// dynamic contiguous arrays
    template <typename T>
    class Vector {
    protected:
        T* buf;
        size_t nItems, capacity;

    public:
        typedef T value_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef T& reference;
        typedef const T& const_reference;

        /// empty constructor
        constexpr Vector() : Vector(nullptr, 0, 0) {}

        /// construct and set the capacity
        explicit Vector(size_t capacity) : Vector(new T[capacity], 0, capacity) {}

        /// construct from initializer list
        Vector(std::initializer_list<T> items) : Vector(new T[items.size()], items.size(), items.size()) {
            if (buf)
                etl::copy(items.begin(), items.end(), begin());
        }

        /// copy constructor
        Vector(const Vector& v) : Vector(v.copy_alloc_(v.capacity), v.nItems, v.capacity) {}

        /// move constructor
        Vector(Vector&& v) noexcept : Vector(v.buf, v.nItems, v.capacity) { v.reset_(); }

        /// copy assignment
        Vector& operator=(const Vector& other) {
            if (this == &other) return *this;

            bool valid = bool(buf) && bool(other.buf);
            if (other.nItems > nItems)
                valid = reserve(other.nItems);
            
            if (valid) {
                etl::copy(other.begin(), other.end(), buf);
                nItems = other.nItems;
            }
            return *this;
        }

        /// move assignment
        Vector& operator=(Vector&& other) noexcept {
            if (this == &other) return *this;
            reset_delete_(other.buf, other.nItems, other.capacity);
            other.reset_();
            return *this;
        }

        /// destructor
        ~Vector() noexcept { reset_delete_(); }

        [[nodiscard]] size_t len() const { return nItems; }     ///< returns the number of items
        [[nodiscard]] size_t size() const { return capacity; }  ///< returns the capacity

        iterator data()   { return buf; }
        iterator begin()  { return buf; }
        iterator end()    { return buf + nItems; }
        reference front() { return buf[0]; }
        reference back()  { return buf[nItems - 1]; }

        const_iterator data()   const { return buf; }
        const_iterator begin()  const { return buf; }
        const_iterator end()    const { return buf + nItems; }
        const_reference front() const { return buf[0]; }
        const_reference back()  const { return buf[nItems - 1]; }

        /// get i-th item by dereference
        /// @warning it will be error null dereference if index is not valid
        reference operator[](int i) { return is_valid_index_(i) ? buf[i] : *static_cast<iterator>(nullptr); }
        const_reference operator[](int i) const { return is_valid_index_(i) ? buf[i] : *static_cast<const_iterator>(nullptr); }

        /// return forward iter object
        Iter<iterator> iter() { return Iter(begin(), end(), 1); }
        Iter<const_iterator> iter() const { return Iter(begin(), end(), 1); }

        /// return reversed iter object
        Iter<iterator> reversed() { return Iter(end() - 1, begin() - 1, -1); }
        Iter<const_iterator> reversed() const { return Iter(end() - 1, begin() - 1, -1); }

        /// return true if buf is not null
        explicit operator bool() const { return buf != nullptr; }

        /// add an item to the vector
        template <typename U, typename = enable_if_t<is_convertible_v<decay_t<U>, T>>>
        void append(U&& other) {
            auto newCapacity = nItems + 1;
            if (capacity < newCapacity) 
                reserve(newCapacity);

            buf[nItems] = etl::forward<U>(other);
            ++nItems;
        }

        /// add all items from another vector to this vector
        void append(const Vector& other) {
            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity) 
                reserve(newCapacity);

            etl::copy(etl::begin(other), etl::end(other), end());
            nItems += etl::len(other);
        }

        /// add all items from another vector to this vector
        void append(Vector&& other) {
            Vector other_ = etl::move(other);
            auto newCapacity = nItems + etl::len(other_);
            if (capacity < newCapacity)
                reserve(newCapacity);

            etl::move(etl::begin(other_), etl::end(other_), end());
            nItems += etl::len(other_);
        }

        /// insert new item given the index
        template <typename U, typename = enable_if_t<is_convertible_v<decay_t<U>, T>>>
        void insert(int index, U&& item) {
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            auto newCapacity = nItems + 1;
            if (capacity < newCapacity)
                reserve(newCapacity);

            auto [x, y] = etl::pair(end(), end() - 1);
            for (; x != &buf[index]; --x, --y) 
                *x = etl::move(*y); // shift

            buf[index] = etl::forward<T>(item);
            ++nItems;
        }

        /// insert another vector given the index
        void insert(int index, const Vector& other) {
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            auto newCapacity = nItems + etl::len(other);
            if (capacity < newCapacity)
                reserve(newCapacity);

            auto [x, y] = etl::pair(begin() + newCapacity - 1, end() - 1);
            for (; x != &buf[index]; --x, --y) 
                *x = etl::move(*y); // shift

            etl::copy(etl::begin(other), etl::end(other), begin() + index);
            nItems += etl::len(other);
        }

        /// insert another vector given the index
        void insert(int index, Vector&& other) {
            Vector other_ = etl::move(other);
            index = index >= 0 ? etl::min(index, int(nItems)) :
                    int(nItems) + etl::max(index, -int(nItems));

            auto newCapacity = nItems + etl::len(other_);
            if (capacity < newCapacity)
                reserve(newCapacity);

            auto [x, y] = etl::pair(begin() + newCapacity - 1, end() - 1);
            for (; x != &buf[index]; --x, --y)
                *x = etl::move(*y); // shift

            etl::move(etl::begin(other_), etl::end(other_), begin() + index);
            nItems += etl::len(other_);
        }

        /// remove an item given the index
        bool remove_at(int index) {
            if (!is_valid_index_(index)) 
                return false;

            for (; index < int(nItems) - 1; ++index)
                buf[index] = buf[index + 1];

            --nItems;
            return true;
        }

        /// remove the first item found in the vector
        bool remove(const_reference x) {
            int index = etl::find(begin(), end(), x) - begin();
            return remove_at(index);
        }

        /// set new capacity, return true if success
        bool reserve(size_t newCapacity) {
            if (newCapacity == 0) {
                reset_delete_();
                return true;
            }
            else if (newCapacity == capacity) {
                return true;
            }

            auto newBuf = move_alloc_(newCapacity);
            if (newBuf == nullptr) 
                return false;

            reset_delete_(newBuf, etl::min(nItems, newCapacity), newCapacity);
            return true;
        }

        /// set n items to 0, capacity remains the same
        void clear() { nItems = 0; }

        /// shrink the capacity to fit the number of items
        bool shrink() { return reserve(nItems); }

        /// expand the number of items to fit the capacity
        void expand() { nItems = capacity; }

        /// slice operator
        Iter<iterator> operator()(int start, int stop, int step = 1) { 
            return (start < stop && step > 0) || (start > stop && step < 0) ? 
                etl::iter(&operator[](start), &operator[](stop), step) : // valid index
                etl::iter(begin(), begin()); // invalid index
        }

        Iter<const_iterator> operator()(int start, int stop, int step = 1) const { 
            return (start < stop && step > 0) || (start > stop && step < 0) ? 
                etl::iter(&operator[](start), &operator[](stop), step) : // valid index
                etl::iter(begin(), begin()); // invalid index
        }

        /// create new vector by adding an item
        template <typename U, typename = enable_if_t<is_convertible_v<decay_t<U>, T>>>
        Vector operator+(U&& other) const {
            auto newCapacity = nItems + 1;
            auto temp = copy_alloc_(newCapacity);
            temp[nItems] = etl::forward<T>(other);
            return { temp, newCapacity, newCapacity }; 
        }

        /// create new vector by adding another vector
        Vector operator+(const Vector& other) const { 
            auto newCapacity = nItems + etl::len(other);
            auto temp = copy_alloc_(newCapacity);
            etl::copy(etl::begin(other), etl::end(other), temp + nItems);
            return { temp, newCapacity, newCapacity }; 
        }

        /// create new vector by adding another vector
        Vector operator+(Vector&& other) const {
            Vector other_ = etl::move(other);
            auto newCapacity = nItems + etl::len(other_);
            auto temp = copy_alloc_(newCapacity);
            etl::move(etl::begin(other_), etl::end(other_), temp + nItems);
            return { temp, newCapacity, newCapacity };
        }

        /// append operator
        template <typename U>
        Vector& operator+=(U&& other) { append(etl::forward<U>(other)); return *this; }
    
    protected:
        Vector(T* buffer, size_t nItems, size_t capacity) 
            : buf(buffer)
            , nItems(buffer ? nItems : 0)
            , capacity(buffer ? capacity : 0) {}

        iterator copy_alloc_(size_t newCapacity) const {
            auto temp = new T[newCapacity];
            if (!temp) return temp;
            etl::copy(buf, buf + etl::min(newCapacity, nItems), temp);
            return temp;
        }

        iterator move_alloc_(size_t newCapacity) const {
            auto temp = new T[newCapacity];
            if (!temp) return temp;
            etl::move(buf, buf + etl::min(newCapacity, nItems), temp);
            return temp;
        }

        void reset_(iterator ptr = nullptr, size_t n = 0, size_t cap = 0) {
            buf = ptr;
            nItems = n;
            capacity = cap;
        }

        void reset_delete_(iterator ptr = nullptr, size_t n = 0, size_t cap = 0) {
            delete[] buf;
            reset_(ptr, n, cap);
        }

        bool is_valid_index_(int& index) const {
            if (nItems == 0) return false;
            if (index < 0) index = int(nItems) + index; // allowing negative index
            if (index < 0 || size_t(index) >= nItems) return false; // out of range
            return true;
        }
    };

    /// create vector with variadic template function, the type can be implicitly or explicitly specified
    template <typename T = void, typename U, typename... Us, typename R = conditional_t<is_void_v<T>, decay_t<U>, T>> auto
    vector(U&& val, Us&&...vals) { return Vector<R> { R(etl::forward<U>(val)), R(etl::forward<Us>(vals))... }; }

    /// create vector from initializer list
    template <typename T> auto
    vector(std::initializer_list<T> items) { return Vector<T>(items); }

    /// create empty vector, capacity is 0
    template <typename T> constexpr auto
    vector() { return Vector<T>(); }

    /// create empty vector and set the capacity
    template <typename T> auto
    vector_reserve(size_t capacity) { return Vector<T>(capacity); }

    /// convert any sequence to a vector
    template <typename Sequence> auto
    vectorize(Sequence&& seq) {
        using T = decay_t<decltype(*etl::begin(etl::declval<Sequence>()))>;
        auto res = vector<T>();

        if constexpr (has_len_v<remove_reference_t<Sequence>>)
            res.reserve(etl::len(seq));

        if constexpr (is_lvalue_reference_v<Sequence>) {
            for (decltype(auto) item : seq)
                res += item;
        } else {
            auto seq_ = etl::move(seq);
            for (decltype(auto) item : seq_)
                res += etl::move(item);
        }

        return res;
    }

    /// type traits
    template <typename T> struct is_vector : false_type {};
    template <typename T> struct is_vector<Vector<T>> : true_type {};
    template <typename T> struct is_vector<const Vector<T>> : true_type {};
    template <typename T> struct is_vector<volatile Vector<T>> : true_type {};
    template <typename T> struct is_vector<const volatile Vector<T>> : true_type {};
    template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

    template <typename T> struct remove_extent<Vector<T>> { typedef T type; };
    template <typename T> struct remove_extent<const Vector<T>> { typedef T type; };
    template <typename T> struct remove_extent<volatile Vector<T>> { typedef T type; };
    template <typename T> struct remove_extent<const volatile Vector<T>> { typedef T type; };
}

#endif //ETL_VECTOR_H
