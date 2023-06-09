/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MSTD_MEMORY_
#define MSTD_MEMORY_

/* "mstd_memory.h"
 *
 * - includes toolchain's <memory>
 * - For ARM C 5, C++11/14 features:
 *   - std::align
 *   - std::addressof
 *   - std::uninitialized_copy_n
 *   - std::unique_ptr, std::make_unique, std::default_delete
 * - For all toolchains, C++17 backports:
 *   - mstd::uninitialized_default_construct, mstd::uninitialized_value_construct
 *   - mstd::uninitialized_move, mstd::uninitialized_move_n
 *   - mstd::destroy_at, mstd::destroy, mstd::destroy_n
 */

#include <memory>

#include "mstd_type_traits.h"
#include "mstd_utility.h" // std::pair
#include "mstd_iterator.h" // std::iterator_traits

namespace mstd {
    using std::align;
    using std::allocator;
    using std::addressof;

    // [uninitialized.construct.default] (C++17)
    template <class ForwardIterator, class Size>
    void uninitialized_default_construct(ForwardIterator first, ForwardIterator last) {
        for (; first != last; ++first) {
            ::new (static_cast<void*>(addressof(*first)))
                    typename std::iterator_traits<ForwardIterator>::value_type;
        }
    }

    template <class ForwardIterator, class Size>
    ForwardIterator uninitialized_default_construct_n(ForwardIterator first, Size n) {
        for (; n; ++first, --n) {
            ::new (static_cast<void*>(addressof(*first)))
                    typename std::iterator_traits<ForwardIterator>::value_type;
        }

        return first;
    }

    // [uninitialized.construct.value] (C++17)
    template <class ForwardIterator, class Size>
    void uninitialized_value_construct(ForwardIterator first, ForwardIterator last) {
        for (; first != last; ++first) {
            ::new (static_cast<void*>(addressof(*first)))
                    typename std::iterator_traits<ForwardIterator>::value_type();
        }
    }

    template <class ForwardIterator, class Size>
    ForwardIterator uninitialized_value_construct_n(ForwardIterator first, Size n) {
        for (; n; ++first, --n) {
            ::new (static_cast<void*>(addressof(*first)))
                    typename std::iterator_traits<ForwardIterator>::value_type();
        }

        return first;
    }

    // [uninitialized.move] (C++17)
    template <class InputIterator, class ForwardIterator>
    ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
        for (; first != last; ++result, (void) ++first) {
            ::new (static_cast<void*>(addressof(*result)))
                  typename std::iterator_traits<ForwardIterator>::value_type(move(*first));
        }

        return result;
    }

    template <class InputIterator, class Size, class ForwardIterator>
    std::pair<InputIterator, ForwardIterator> uninitialized_move_n(InputIterator first, Size n, ForwardIterator result) {
        for ( ; n > 0; ++result, (void) ++first, --n) {
            ::new (static_cast<void*>(addressof(*result)))
                  typename std::iterator_traits<ForwardIterator>::value_type(std::move(*first));
        }

        return { first, result };
    }

    using std::uninitialized_copy;
    using std::uninitialized_copy_n;
    using std::uninitialized_fill;
    using std::uninitialized_fill_n;

    // [specialized.destroy] (C++17)
    template <class T>
    void destroy_at(T *location)
    {
        location->~T();
    }

    template <class ForwardIterator>
    void destroy(ForwardIterator first, ForwardIterator last)
    {
        for (; first != last; ++first) {
            destroy_at(addressof(*first));
        }
    }

    template <class ForwardIterator, class Size>
    ForwardIterator destroy_n(ForwardIterator first, Size n)
    {
        for (; n > 0; (void)++first, --n) {
            destroy_at(addressof(*first));
        }
        return first;
    }

    using std::default_delete;
    using std::unique_ptr;
    using std::make_unique;
}

#endif // MSTD_MEMORY_
