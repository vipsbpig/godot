#ifndef SOLHELPER_H
#define SOLHELPER_H
#include "core/reference.h"
#include "core/object.h"

namespace sol {
template <typename T>
    struct unique_usertype_traits<Ref<T>> {
        typedef T type;
        typedef Ref<T> actual_type;

        static const bool value = true;

        static bool is_null(const actual_type& p) {
            return p == nullptr;
        }

        static type* get(const actual_type& p) {
            return p.ptr();
        }
    };


}

#endif // SOLHELPER_H
