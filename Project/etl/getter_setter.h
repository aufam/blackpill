#ifndef ETL_GETTER_SETTER_H
#define ETL_GETTER_SETTER_H

#include "etl/utility_basic.h"

namespace Project::etl {

    template <typename T, typename GetterFunction>
    class Getter {
        GetterFunction get;

    public:
        constexpr Getter(GetterFunction get) : get(get) {}        
      
        constexpr operator T() const { return get(); }

        template <typename U>
        constexpr decltype(auto) operator==(U&& value) const { return get() == etl::forward<U>(value); }

        template <typename U>
        void operator=(U&&) = delete;
    };
    

    template <typename T, typename SetterFunction>
    class Setter {
        SetterFunction set;

    public:
        constexpr Setter(SetterFunction set) : set(set) {}

        template <typename U>
        constexpr decltype(auto) operator=(U&& value) const { return set(etl::forward<U>(value)); }

        template <typename U>
        bool operator==(U&& value) = delete;

        operator T() = delete;
    };


    template <typename T, typename GetterFunction, typename SetterFunction>
    class GetterSetter {
        GetterFunction get;
        SetterFunction set;

    public:
        constexpr GetterSetter(GetterFunction get, SetterFunction set) : get(get), set(set) {}

        constexpr operator T() const { return get(); }

        template <typename U>
        constexpr decltype(auto) operator==(U&& value) const { return get() == etl::forward<U>(value); }

        template <typename U>
        constexpr decltype(auto) operator=(U&& value) const { return set(etl::forward<U>(value)); }
    };


    template <typename T, typename GetterFunction> constexpr auto
    make_getter(GetterFunction&& get) { return Getter<T, GetterFunction> { etl::forward<GetterFunction>(get) }; }

    template <typename T, typename SetterFunction> constexpr auto
    make_setter(SetterFunction&& set) { return Setter<T, SetterFunction> { etl::forward<SetterFunction>(set) }; }

    template <typename T, typename GetterFunction, typename SetterFunction> constexpr auto
    make_getter_setter(GetterFunction&& get, SetterFunction&& set) { 
        return GetterSetter<T, GetterFunction, SetterFunction> { 
            etl::forward<GetterFunction>(get), 
            etl::forward<SetterFunction>(set) 
        }; 
    }

}

#endif