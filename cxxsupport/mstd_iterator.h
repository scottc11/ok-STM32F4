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
#ifndef MSTD_ITERATOR_
#define MSTD_ITERATOR_

/* "mstd_iterator.h"
 *
 * - includes toolchain's <iterator>
 * - For ARM C 5, C++11/14 features:
 *   - std::begin, std::end, etc
 *   - std::make_reverse_iterator
 *   - std::move_iterator, std::make_move_iterator
 * - For all toolchains, C++17/20 backports:
 *   - mstd::size
 *   - mstd::ssize
 *   - mstd::empty
 *   - mstd::data
 *   - mstd::iter_reference_t
 */

#include <iterator>
#include "mstd_type_traits.h"

namespace mstd {
using std::initializer_list;
using std::iterator_traits;
// omitting deprecated std::iterator
using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;
using std::advance;
using std::distance;
using std::next;
using std::prev;
using std::reverse_iterator;
using std::make_reverse_iterator;
using std::back_insert_iterator;
using std::back_inserter;
using std::front_insert_iterator;
using std::front_inserter;
using std::insert_iterator;
using std::inserter;
using std::move_iterator;
using std::make_move_iterator;
using std::istream_iterator;
using std::ostream_iterator;
using std::istreambuf_iterator;
using std::ostreambuf_iterator;
using std::begin;
using std::end;
using std::cbegin;
using std::cend;
using std::rbegin;
using std::rend;
using std::crbegin;
using std::crend;

#if __cpp_lib_nonmember_container_access >= 201411
using std::size;
using std::empty;
using std::data;
#else
// [iterator.container]
template <class C>
constexpr auto size(const C &c) -> decltype(c.size())
{
    return c.size();
}

template <class T, size_t N>
constexpr size_t size(const T (&)[N]) noexcept
{
    return N;
}

template <class C>
constexpr auto empty(const C &c) -> decltype(c.empty())
{
    return c.empty();
}

template <class T, size_t N>
constexpr bool empty(T (&)[N]) noexcept
{
    return false;
}

template <class E>
constexpr bool empty(initializer_list<E> il)
{
    return il.size() == 0;
}

template <class C>
constexpr auto data(C &c) -> decltype(c.data())
{
    return c.data();
}

template <class C>
constexpr auto data(const C &c) -> decltype(c.data())
{
    return c.data();
}

template <class T, size_t N>
constexpr T *data(T (&array)[N]) noexcept
{
    return array;
}

template <class E>
constexpr const E *data(initializer_list<E> il)
{
    return il.begin();
}
#endif

// C++20
template <class C>
constexpr auto ssize(const C &c) -> common_type_t<ptrdiff_t, make_signed_t<decltype(c.size())>>
{
    return static_cast<common_type_t<ptrdiff_t, make_signed_t<decltype(c.size())>>>(c.size());
}

template <class T, ptrdiff_t N>
constexpr ptrdiff_t ssize(const T (&)[N]) noexcept
{
    return N;
}

template <typename T>
using iter_reference_t = decltype(*std::declval<T&>());

}


#endif // MSTD_ITERATOR_
