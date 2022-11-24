#ifndef ETL_LINKED_LIST_H
#define ETL_LINKED_LIST_H

#include "main.h"

#ifndef ETL_LINKED_LIST_USE_MUTEX
#define ETL_LINKED_LIST_USE_MUTEX 1
#endif

#if ETL_LINKED_LIST_USE_MUTEX == 1
#include "mutex.h"
#define ETL_LINKED_LIST_SCOPE_LOCK() MutexScope lock(mutex)
#else
#define ETL_LINKED_LIST_SCOPE_LOCK()
#endif

namespace Project::etl {

    /// simple linked list class. every item will be allocated to heap
    /// @tparam T item type
    template <class T>
    struct LinkedList {
        typedef T Type; /// item type

        /// contains the item and pointer to next and prev item
        struct Node {
            T item;
            Node* next;
            Node* prev;
            Node() = default;
            explicit Node(const T& item) : item(item), next(nullptr), prev(nullptr) {}
        };

        /// iterator class to modify increment, decrement, bool operator, and dereference of Node*
        struct Iterator {
            Node* node;
            Iterator(Node* node) : node(node) {}
            explicit Iterator(const T& item) : node(new Node(item)) {}

            T* item()       const { return node ? &node->item : nullptr; }
            Iterator next() const { return { node ? node->next : nullptr }; }
            Iterator prev() const { return { node ? node->prev : nullptr }; }

            /// dereference operator
            /// @warning make sure node is not null
            /// @{
            T& operator *() { return node->item; }
            const T& operator *() const { return node->item; }

            T& operator[](int pos) { return *((*this) + pos); }
            const T& operator[](int pos) const { return *((*this) + pos); }
            /// @}

            bool operator ==(const Iterator& other) const { return node == other.node; }
            bool operator !=(const Iterator& other) const { return node != other.node; }
            explicit operator bool()                const { return node != nullptr; }

            Iterator& operator +=(int pos) {
                if (pos > 0)
                    for (; pos > 0 && *this; (*this)++) pos--;
                else
                    for (; pos < 0 && *this; (*this)--) pos++;
                return *this;
            }

            Iterator operator +(int pos) const {
                Iterator res { node };
                if (pos > 0)
                    for (; pos > 0 && res; res++) pos--;
                else
                    for (; pos < 0 && res; res--) pos++;
                return res;
            }

            Iterator& operator -=(int pos) { return operator+=(-pos); }
            Iterator operator -(int pos) const { return operator+(-pos); }

            /// prefix increment
            Iterator& operator ++() {
                *this = next();
                return *this;
            }
            /// postfix increment
            Iterator operator ++(int) {
                Iterator res = *this;
                *this = next();
                return res;
            }
            /// prefix decrement
            Iterator& operator --() {
                *this = prev();
                return *this;
            }
            /// postfix decrement
            Iterator operator --(int) {
                Iterator res = *this;
                *this = prev();
                return res;
            }

            /// insert other iterator to the next/prev of this iterator
            /// @param other other iterator
            /// @param nextOrPref false: insert to next (default), true: insert to prev
            /// @retval 0: fail, 1: success
            /// @note other node and this node can't be null. other next and other prev have to be null
            int insert(Iterator other, bool nextOrPref = false) {
                if (!node || !other || other.next() || other.prev()) return 0;
                auto nx = node->next;
                auto pv = node->prev;
                if (!nextOrPref) {
                    other.node->prev = node;
                    node->next = other.node;
                    other.node->next = nx;
                    if (nx) nx->prev = other.node;
                    return 1;
                }
                other.node->prev = pv;
                if (pv) pv->next = other.node;
                other.node->next = node;
                node->prev = other.node;
                return 1;
            }

            /// detach this iterator from its next and prev iterator
            /// @retval 1: success, 0: already detached or this node = null
            int detach() {
                auto nx = next().node;
                auto pv = prev().node;
                if (nx) nx->prev = pv;
                if (pv) pv->next = nx;
                if (node) node->next = nullptr;
                if (node) node->prev = nullptr;
                return nx || pv;
            }

            /// detach and delete this iterator
            /// @retval 1: success, 0: already detached or this node = null
            int erase() {
                int res = detach();
                delete node;
                node = nullptr;
                return res;
            }
        };

    private:
        mutable Iterator head;
#if ETL_LINKED_LIST_USE_MUTEX == 1
        mutable Mutex mutex{};
#endif

    public:
        constexpr LinkedList() : head(nullptr) {}
#if ETL_LINKED_LIST_USE_MUTEX == 1
        int init()   const { return mutex.init(); } /// init mutex
        int deinit() const { clear(); return mutex.deinit(); } /// clear all item and deinit mutex
#else
        int init()   const { return osOK; }
        int deinit() const { clear(); return osOK; }
#endif

        Iterator data()  const { return head; }
        Iterator begin() const { return head; }
        Iterator end()   const { return { nullptr }; }
        Iterator tail()  const { return head + (len() - 1); } /// last iterator in the linked list

        /// @retval number of items
        size_t len() const {
            size_t res = 0;
            for (const auto& _ : *this) res++;
            return res;
        }

        /// get the first item
        /// @warning return by copy, to avoid null dereference
        T front() const { auto iter = head; return iter ? *iter : T {}; }

        /// get the last item
        /// @warning return by copy, to avoid null dereference
        T back() const { auto iter = tail(); return iter ? *iter : T {}; }

        /// get the i-th item
        /// @warning return by copy, to avoid null dereference
        T operator [](size_t i) const { auto iter = head + i; return iter ? *iter : T {}; }

        /// delete all items
        void clear() const { while (pop()); }

        /// push an item to the tail
        /// @param[in] item the item
        /// @retval 1: success, 0: failed
        int push(const T& item) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            auto node = Iterator(item);
            if (!node) return 0;
            if (!head) {
                head = node;
                return 1;
            }
            return tail().insert(node);
        }

        /// push an item given the position
        /// @param[in] item the item
        /// @param pos position relative to the head
        /// @retval 1: success, 0: failed
        int push(const T& item, size_t pos) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            auto node = Iterator(item);
            if (!node) return 0;
            if (pos == 0) {
                head.insert(node, true);
                head = node;
                return 1;
            }
            int res = (head + (pos - 1)).insert(node);
            if (res == 0) node.erase();
            return res;
        }

        int pushBack(const T& item)  const { return push(item); }    /// push an item to the tail
        int pushFront(const T& item) const { return push(item, 0); } /// push an item to the head

        /// get an item given the position and delete it from the linked list
        /// @param[out] item the item
        /// @param pos position relative to the head
        /// @retval 1: success, 0: failed
        int pop(T& item, size_t pos = 0) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            auto node = head + pos;
            if (node) item = *node.item();
            if (pos == 0) head = head.next();
            return node.erase();
        }

        int pop()             const { T dummy = {}; return pop(dummy, 0); } /// delete first item
        int popBack()         const { T dummy = {}; return pop(dummy, len() - 1); } /// delete last item
        int popFront()        const { T dummy = {}; return pop(dummy, 0); } /// delete first item
        int popBack(T& item)  const { return pop(item, len() - 1); } /// pop the last item
        int popFront(T& item) const { return pop(item, 0); } /// pop the first item

        /// push operator
        const LinkedList& operator <<(const T& item) const { push(item); return *this; }
        /// pop operator
        const LinkedList& operator >>(T& item)       const { pop(item); return *this; }
        explicit operator bool()                     const { return head; }

        /// set all item value
        void fill(const T& item) {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (T& it : *this) it = item;
        }

        /// perform fn(result, item) for each item in this linked list
        /// @tparam R result type
        /// @param result[in,out] result
        /// @param fn function pointer
        /// @{
        template <class R>
        void fold(R& result, void (* fn)(R&, T&)) {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (T& item : *this) fn(result, item);
        }
        template <class R>
        void fold(R& result, void (* fn)(R&, const T&)) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (const T& item : *this) fn(result, item);
        }
        /// @}

        /// perform fn(item) for each item in this linked list
        /// @param fn function pointer
        /// @{
        void foreach(void (* fn)(T&)) {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (T& item : *this) fn(item);
        }
        void foreach(void (* fn)(const T&)) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (const T& item : *this) fn(item);
        }
        /// @}

        /// check any
        /// @retval if one of the items matches the condition
        bool any(bool (*check)(T&)) {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (T& item : *this) if (check(item)) return true;
            return false;
        }
        bool any(bool (*check)(const T&)) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (const T& item : *this) if (check(item)) return true;
            return false;
        }
        bool any(const T& check) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (const T& item : *this) if (check == item) return true;
            return false;
        }
        /// @}

        /// check all
        /// @retval if all the items matches the condition
        bool all(bool (*check)(T&)) {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (T& item : *this) if (!check(item)) return false;
            return true;
        }
        bool all(bool (*check)(const T&)) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (const T& item : *this) if (!check(item)) return false;
            return true;
        }
        bool all(const T& check) const {
            ETL_LINKED_LIST_SCOPE_LOCK();
            for (const T& item : *this) if (check != item) return false;
            return true;
        }
        /// @}
    };

}

#endif //ETL_LINKED_LIST_H
