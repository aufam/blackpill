// #ifndef ETL_KEYWORDS_H
// #define ETL_KEYWORDS_H

/*
 * warning!!!
 * - using custom keywords is generally considered as bad practice
 * - custom keywords may conflict with existing source codes or libraries
 * - don't put this into a header file (hence, no header guard)
 * - don't put this before other header files
 * - define ETL_DISABLE_MORE_KEYWORDS to disable non-standard keywords
 */

#include "etl/utility.h" // pair, triple, len, next, iter, enumerate, zip, range
#include <ciso646>       // or, and, not, not_eq


#ifdef ETL_ENABLE_EXTRA_KEYWORDS

#include "etl/getter_setter.h"
#include "etl/function.h"

#define ETL_SETUP_GETTER_SETTER(CLASS)                                                  \
    template <typename T>                                                               \
    using Getter = Project::etl::Getter<T, Project::etl::Function<T(), CLASS*>>;        \
                                                                                        \
    template <typename T>                                                               \
    using Setter = Project::etl::Setter<T, Project::etl::Function<void(T), CLASS*>>;    \
                                                                                        \
    template <typename T>                                                               \
    using GetterSetter = Project::etl::GetterSetter<T, Project::etl::Function<T(), CLASS*>, Project::etl::Function<void(T), CLASS*>>;

#define constructor         // for class constructor
#define implicit            // for implicit conversion
#define empty_constructor   // for empty constructor
#define copy_constructor    // for copy constructor
#define move_constructor    // for move constructor
#define destructor ~        // for class destructor
#define abstract            // for a class that is not intended to be instantiated on its own but serves as a blueprint or template for other classes
#define extendable          // for a class that can be instantiated and inherited from
#define final               // for a class that is not intended to be inherited from
#define extends : public    // inherits public member of other class

#define namespace_start(name) namespace Project::name {
#define namespace_end }

#endif // ETL_ENABLE_EXTRA_KEYWORDS


#ifndef ETL_DISABLE_MORE_KEYWORDS

#define in :            // python's in (only for iterating a container)
#define elif else if    // python's elif
#define is ==           // python's is (should be used only for comparing a variable to a literal)
#define is_not !=

#define null Project::etl::none // kotlin's null
#define var auto        // kotlin's var
#define val const auto  // kotlin's val
#define fun auto        // kotlin's fun (should be used to define a function)

#define lambda []       // capture-less lambda expression
#define _ dummy__[[maybe_unused]] // unused variable

#endif // ETL_DISABLE_MORE_KEYWORDS

// #endif //ETL_KEYWORDS_H
