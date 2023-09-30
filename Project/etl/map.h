#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/vector.h"

namespace Project::etl {

    /// collection of key-value pairs, keys are unique
    template <typename K, typename V>
    class Map : public Vector<Pair<K, V>> {
        Map(Pair<K, V>* buffer, size_t nItems, size_t capacity) : Vector<Pair<K, V>>(buffer, nItems, capacity) {}

    public:
        typedef K Key;
        typedef V Value;

        /// empty constructor
        constexpr Map() : Vector<Pair<K, V>>() {}

        /// construct and set the capacity
        Map(size_t capacity) : Map(new Pair<K, V>[capacity], 0, capacity) {}

        /// construct from initializer list
        Map(std::initializer_list<Pair<K, V>> items) : Map(new Pair<K, V>[items.size()], 0, items.size()) {
            for (auto item : items) {
                auto vp = get_value_ptr_(item.x);
                if (vp) *vp = item.y;
                else this->buf[this->nItems++] = item;
            }
        }

        /// copy constructor
        Map(const Map& m) : Map(m.copy_alloc_(m.capacity), m.nItems, m.capacity) {}

        /// move constructor
        Map(Map&& m) noexcept : Map(etl::move(m.buf), etl::move(m.nItems), etl::move(m.capacity)) { m.reset_(); }

        /// copy assignment
        Map& operator=(const Map& other) {
            if (this == &other) return *this;
            this->reserve(other.capacity);
            etl::copy(other.begin(), other.end(), this->buf);
            return *this;
        }

        /// move assignment
        Map& operator=(Map&& other) noexcept {
            if (this == &other) return *this;
            this->reset_delete_(etl::move(other.buf), etl::move(other.nItems), etl::move(other.capacity));
            other.reset_();
            return *this;
        }

        /// check if a key is in this map
        bool has(const K& key) const { return static_cast<bool>(get_value_ptr_(key)); }

        /// get a value given the key
        /// @warning it will throw error null dereference if key does not exist
        V& get(const K& key) { return *get_value_ptr_(key); }

        /// get a value given the key
        /// @warning it will throw error null dereference if key does not exist
        const V& get(const K& key) const { return *get_value_ptr_(key); }

        /// get a value given the key. if the key does not exist, append new pair and return default constructed V
        template <typename KK>
        V& operator[](KK&& key) {
            auto res = get_value_ptr_(key);
            if (res) return *res;
            append_force_(etl::forward<KK>(key), Value{});
            return this->back().y;
        }

        /// get a value given the key. if the key does not exist, return default constructed V
        const V& operator[](const K& key) const {
            auto res = get_value_ptr_(key);
            if (res) return *res;
            static const auto v = Value{};
            return v;
        }
 
        /// add a key-value pair to the map
        void append(const Pair<K, V>& other) { operator[](other.x) = other.y; }
        Map& operator+=(const Pair<K, V>& other) { append(other); return *this; }

        /// add all items from other map 
        void append(const Map& other) { for (auto &pair : other) append(pair); }
        Map& operator+=(const Map& other) { append(other); return *this; }

        /// create copy of this map and add a key-value pair
        Map operator+(const Pair<K, V>& other) const { 
            auto res = *this;
            res += other;
            return etl::move(res);
        }

        /// create copy of this map and add another map
        Map operator+(const Map& other) const {
            auto res = *this;
            res += other; 
            return etl::move(res);
        }

        /// remove key-value pair given the key
        bool remove(const K& key) {
            int index = 0;
            for (auto& pair : *this) {
                if (pair.x == key) break;
                ++index;
            }
            return this->remove_at(index);
        }
    
    private:
        V* get_value_ptr_(const K& key) {
            V* res = nullptr;
            for (auto &[x, y]: *this) if (x == key) res = &y;
            return res;
        }

        const V* get_value_ptr_(const K& key) const {
            const V* res = nullptr;
            for (auto &[x, y]: *this) if (x == key) res = &y;
            return res;
        }

        template <typename KK, typename VV>
        void append_force_(KK&& key, VV&& value) {
            auto newCapacity = this->nItems + 1;

            if (this->capacity < newCapacity)
                this->reserve(newCapacity);
            
            this->buf[this->nItems] = etl::pair<K, V>(K(etl::forward<KK>(key)), V(etl::forward<VV>(value)));
            ++this->nItems;
        }
    };

    /// create map using variadic function, type is implicitly specified
    template <typename K, typename V, typename... Ks, typename... Vs>
    enable_if_t<(is_same_v<K, Ks> && ...) && (is_same_v<V, Vs> && ...), Map<K, V>>
    map(const Pair<K, V>& item, const Pair<Ks, Vs>&... items) { return Map<K, V> { item, items... }; }

    /// create map from initializer list, type is explicitly specified
    template <typename K, typename V> constexpr auto
    map(std::initializer_list<Pair<K,V>> items) { return Map<K, V>(items); }

    /// create empty map, capacity is 0
    template <typename K, typename V> constexpr auto
    map() { return Map<K, V> {}; }

    /// create empty map, and set the capacity
    template <typename K, typename V> Map<K, V>
    map_reserve(size_t capacity) { return Map<K, V>(capacity); }

    /// type traits
    template <typename T> struct is_map : false_type {};
    template <typename K, typename V> struct is_map<Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<const Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<volatile Map<K, V>> : true_type {};
    template <typename K, typename V> struct is_map<const volatile Map<K, V>> : true_type {};
    template <typename T> inline constexpr bool is_map_v = is_map<T>::value;

    template <typename K, typename V> struct remove_extent<Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<const Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<volatile Map<K, V>> { typedef Pair<K, V> type; };
    template <typename K, typename V> struct remove_extent<const volatile Map<K, V>> { typedef Pair<K, V> type; };
}

#endif //ETL_MAP_H
