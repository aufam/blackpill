#ifndef ETL_LINKED_LIST_H
#define ETL_LINKED_LIST_H

#include "etl/algorithm.h"

namespace Project::etl {

    /// doubly linked list. every item will be allocated to heap
    template <typename T>
    class LinkedList {
        struct Node; ///< contains the item and pointer to next and prev items

    public:

        template <typename U>
        class Iterator;

        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef Iterator<Node*> iterator;
        typedef Iterator<const Node*> const_iterator;

        /// empty constructor
        constexpr LinkedList() {}

        /// variadic template function constructor
        template <typename U, typename ...Us>
        explicit LinkedList(U&& item, Us&&... items) { 
            push(etl::forward<U>(item));
            ((push(etl::forward<Us>(items))), ...);
        }

        /// construct from initializer list
        LinkedList(std::initializer_list<T> items) {
            for (auto& item : items) push(item);
        }

        /// copy constructor
        LinkedList(const LinkedList& l) {
            for (auto& item : l) push(item);
        }

        /// move constructor
        LinkedList(LinkedList&& l) noexcept : head(etl::exchange(l.head, iterator())) {}

        /// copy assignment
        LinkedList& operator=(const LinkedList& other) {
            for (auto i : etl::range(other.len())) operator[](i) = other[i];
            return *this;
        }

        /// move assignment
        LinkedList& operator=(LinkedList&& other) noexcept {
            clear();
            head = etl::exchange(other.head, iterator());
            return *this;
        }

        ~LinkedList() { clear(); }

        iterator data()  { return head; }
        iterator begin() { return head; }
        iterator end()   { return iterator(); }
        iterator tail()  { return head + (len() - 1); }

        const_iterator data()  const { return const_iterator(head); }
        const_iterator begin() const { return const_iterator(head); }
        const_iterator end()   const { return const_iterator(); }
        const_iterator tail()  const { return const_iterator(head) + (len() - 1); }

        /// @retval number of items
        size_t len() const {
            size_t res = 0;
            for (auto& _ [[maybe_unused]] : *this) res++;
            return res;
        }

        /// delete all items
        void clear() const { while (pop()); }

        /// get the first item
        /// @warning make sure head is not null
        reference front() { return *head; }

        /// get the first item
        /// @warning make sure head is not null
        const_reference front() const { return *const_iterator(head); }

        /// get the last item
        /// @warning make sure tail() is not null
        reference back() { return *tail(); }

        /// get the last item
        /// @warning make sure tail() is not null
        const_reference back() const { return *tail(); }

        /// get i-th item by dereference
        /// @warning if head + i = null, it will make new node and return its item
        /// @warning if i less than 0, it will iterate backward from the tail
        reference operator[](int i) {
            auto p = i >= 0 ? head + i : tail() + (i + 1);
            if (p) return *p;
            T dummy = {};
            i >= 0 ? push_back(dummy) : push_front(dummy);
            return i >= 0 ? back() : front();
        }

        /// get i-th item by dereference
        /// @warning if head + i = null, it will make new node and return its item
        /// @warning if i less than 0, it will iterate backward from the tail
        const_reference operator[](int i) const {
            auto p = i >= 0 ? const_iterator(head) + i : tail() + (i + 1);
            if (p) return *p;
            T dummy = {};
            i >= 0 ? push_back(dummy) : push_front(dummy);
            return i >= 0 ? back() : front();
        }

        explicit operator bool() const { return bool(head); }

        /// slice operator
        Iter<iterator> operator()(int start, int stop, int step = 1) { 
            auto b = start >= 0 ? head + start : tail() + (start + 1);
            auto e = stop >= 0 ? head + stop : tail() + (stop + 1);
            return Iter(b, e, step);
        }

        /// slice operator
        Iter<const_iterator> operator()(int start, int stop, int step = 1) const { 
            auto b = start >= 0 ? const_iterator(head) + start : tail() + (start + 1);
            auto e = stop >= 0 ? const_iterator(head) + stop : tail() + (stop + 1);
            return Iter(b, e, step);
        }

        Iter<iterator> iter() { return Iter(begin(), end(), 1); }

        Iter<const_iterator> iter() const { return Iter<const_iterator>(begin(), end(), 1); }

        Iter<iterator> reversed() { return Iter(tail(), end(), -1); }

        Iter<const_iterator> reversed() const { return Iter<const_iterator>(tail(), end(), -1); }

        /// push operator
        template <typename U>
        const LinkedList& operator<<(U&& item) const { push(etl::forward<U>(item)); return *this; }

        /// pop operator
        const LinkedList& operator>>(reference item) const { pop(item); return *this; }

        /// push an item at the back of the list
        template <typename U>
        int push(U&& item) const {
            auto node = make_iterator(etl::forward<U>(item)); // create new node
            if (!node) 
                return 0; 

            if (!head) { 
                head = node; // if the linked list is empty, set the head to the new node
                return 1;
            }

            return tail().insert(const_iterator(node)); // insert the node at the back
        }

        /// push an item to the list at a specific position
        template <typename U>
        int push(U&& item, size_t pos) const {
            auto node = make_iterator(etl::forward<U>(item)); // create new node
            if (!node) 
                return 0;

            if (pos == 0) {
                head.insert_prev(node);  // if pos is 0, insert the new node at the beginning
                head = node;
                return 1;
            }

            int res = (head + (pos - 1)).insert(node); // find the node at the desired position and insert the new node after it
            if (res == 0) 
                node.erase(); // erase the new node if insertion failed

            return res;
        }

        /// push an item at the back
        template <typename U>
        int push_back(U&& item)  const { return push(etl::forward<U>(item)); }

        /// push an item at the front
        template <typename U>
        int push_front(U&& item) const { return push(etl::forward<U>(item), 0); }

        /// removes an item from the list at a specific position and retrieves its value
        int pop_at(reference item, size_t pos) const {
            auto node = head + pos;
            if (node) 
                item = etl::move(*node);
            
            if (pos == 0 && head) 
                head = head.next();
            
            return node.erase();
        }

        /// removes an item from the list at a specific position
        int pop_at(size_t pos) const { 
            auto node = head + pos;
            if (pos == 0 && head) 
                head = head.next();
            
            return node.erase();
         }

        /// removes the first item from the list and retrieves its value
        int pop(reference item) const { return pop_at(item, 0); }

        /// removes the first item from the list
        int pop() const { return pop_at(0); }

        /// removes the first item from the list
        int pop_back() const { return pop_at(len() - 1); }

        /// removes the first item from the list
        int pop_front() const { return pop_at(0); }
        
        /// removes the first item from the list
        int pop_back(T& item)  const { return pop_at(item, len() - 1); }

        /// removes the first item from the list
        int pop_front(T& item) const { return pop_at(item, 0); }

    private:
        mutable iterator head;

        /// make iterator
        template <typename U> static auto
        make_iterator(U&& item) { return iterator(new Node(etl::forward<U>(item))); }
    };

    /// create linked list with variadic template function, the type can be implicitly or explicitly specified
    template <typename T = void, typename U, typename... Us, typename R = conditional_t<is_void_v<T>, decay_t<U>, T>> auto
    list(U&& val, Us&&... vals) { return LinkedList<R> { R(etl::forward<U>(val)), R(etl::forward<Us>(vals))... }; }

    /// create linked list from initializer list
    template <typename T> auto
    list(std::initializer_list<T> items) { return LinkedList<T>(items); }

    /// empty linked list
    template <typename T> auto 
    list() { return LinkedList<T> {}; }

    template <typename T>
    struct LinkedList<T>::Node {
        T item;
        mutable Node* next = nullptr;
        mutable Node* prev = nullptr;
        
        explicit Node(const T& item) : item(item) {}
        explicit Node(T&& item) : item(etl::move(item)) {}
    };

    template <typename T>
    template <typename U>
    class LinkedList<T>::Iterator {
        static_assert(is_same_v<U, Node*> || is_same_v<U, const Node*>, "the iterator has to be node pointer");
        friend class LinkedList<T>;
        Node* node;

        /// construct from node pointer
        Iterator(Node* node) : node(node) {} // NOLINT

    public:
        /// empty constructor
        Iterator() : node(nullptr) {}

        /// copy constructor
        template <typename V>
        explicit Iterator(const Iterator<V>& other) : node(other.node) {}

        /// move constructor
        template <typename V>
        explicit Iterator(Iterator<V>&& other) noexcept : node(etl::exchange(other.node, nullptr)) {}

        /// copy assignment
        template <typename V>
        Iterator& operator=(const Iterator<V>& other) {
            if (*this == other) return *this;
            node = other.node; 
            return *this;
        }

        /// move assignment
        template <typename V>
        Iterator& operator=(Iterator<V>&& other) noexcept {
            if (*this == other) return *this;
            node = etl::exchange(other.node, nullptr); 
            return *this;
        }

        /// return true if the node is not null
        explicit operator bool() const { return node != nullptr; }

        /// get i-th item by dereference
        /// @warning make sure node + i is not null
        auto& operator[](int i) const {
            if constexpr (is_const_v<remove_pointer_t<U>>) {
                const auto it = (*this) + i;
                return *it;
            } else {
                auto it = (*this) + i;
                return *it;
            }
        }

        /// arrow operator to access the item's member
        auto* operator->() const {
            if constexpr (is_const_v<remove_pointer_t<U>>) {
                const auto it = node ? &node->item : nullptr;
                return it;
            } else {
                auto it = node ? &node->item : nullptr;
                return it;
            }
        }
        
        /// dereference operator
        /// @warning make sure node is not null
        auto& operator*() const {
            if constexpr (is_const_v<remove_pointer_t<U>>) {
                const auto& it = node->item;
                return it;
            } else {
                auto& it = node->item;
                return it;
            }
        }

        /* comparison operators */

        template <typename V>
        bool operator==(Iterator<V> other) const { return node == other.node; }

        template <typename V>
        bool operator!=(Iterator<V> other) const { return node != other.node; }

        bool operator==(std::nullptr_t) const { return node == nullptr; }
        bool operator!=(std::nullptr_t) const { return node != nullptr; }

        /* arithmetic operators */

        Iterator operator+(int pos) const {
            Iterator res { node };
            if (pos > 0)
                for (; pos > 0 && res; res = res.next()) pos--;
            else if (pos < 0)
                for (; pos < 0 && res; res = res.prev()) pos++;
            return res;
        }
        
        Iterator operator-(int pos) const {
            return *this + (-pos);
        }

        Iterator& operator+=(int pos) {
            *this = *this + pos;
            return *this;
        }
        
        Iterator& operator-=(int pos) {
            *this = *this - pos;
            return *this;
        }

        Iterator& operator++() {
            if (node) node = node->next;
            return *this;
        }
        
        Iterator operator++(int) { // NOLINT
            Iterator res { node };
            if (node) node = node->next;
            return res;
        }

        Iterator& operator--() {
            if (node) node = node->prev;
            return *this;
        }
        
        Iterator operator--(int) { // NOLINT
            Iterator res { node };
            if (node) node = node->prev;
            return res;
        }

        size_t operator-(Iterator other) const {
            size_t i = 0;
            if (node == nullptr) {
                for (; other; ++other, ++i);
                return i;
            }
            for (auto p = *this; p && p != other; --p, ++i);
            return i;
        }

    private:
        Iterator next() const { return { node ? node->next : nullptr }; }
        Iterator prev() const { return { node ? node->prev : nullptr }; }
        
        /// insert another iterator to the next/prev position of this iterator
        /// @param other other iterator
        /// @param nextOrPref false: insert to next (default), true: insert to prev
        /// @return 0: fail, 1: success
        /// @note other node and this node can't be null. other node's next and other node's prev have to be null
        int insert(Iterator other, bool nextOrPref = false) {
            if (!node || !other || other.next() || other.prev()) 
                return 0; // check requirements

            auto nx = next().node;
            auto pv = prev().node;

            // insert next
            if (!nextOrPref) {
                other.node->prev = node;
                node->next = other.node;
                other.node->next = nx;
                if (nx) nx->prev = other.node;
                return 1;
            }

            // insert prev
            other.node->prev = pv;
            if (pv) pv->next = other.node;
            other.node->next = node;
            node->prev = other.node;
            return 1;
        }

        /// insert other iterator to the prev position of this iterator
        int insert_prev(Iterator other) { return insert(other, true); }

        /// detach this iterator from its next and prev iterator
        /// @retval 1: success, 0: already detached or this node = null
        int detach() {
            auto nx = next().node;
            auto pv = prev().node;
            if (nx) nx->prev = pv;
            if (pv) pv->next = nx;
            return nx || pv;
        }

        /// detach and delete this iterator
        int erase() {
            int res = detach();
            delete node;
            node = nullptr;
            return res;
        }
    };

    /// type traits
    template <typename T> struct is_linked_list : false_type {};
    template <typename T> struct is_linked_list<LinkedList<T>> : true_type {};
    template <typename T> struct is_linked_list<const LinkedList<T>> : true_type {};
    template <typename T> struct is_linked_list<volatile LinkedList<T>> : true_type {};
    template <typename T> struct is_linked_list<const volatile LinkedList<T>> : true_type {};
    template <typename T> inline constexpr bool is_linked_list_v = is_linked_list<T>::value;

    template <typename T> struct remove_extent<LinkedList<T>> { typedef T type; };
    template <typename T> struct remove_extent<const LinkedList<T>> { typedef T type; };
    template <typename T> struct remove_extent<volatile LinkedList<T>> { typedef T type; };
    template <typename T> struct remove_extent<const volatile LinkedList<T>> { typedef T type; };
}

#endif //ETL_LINKED_LIST_H
