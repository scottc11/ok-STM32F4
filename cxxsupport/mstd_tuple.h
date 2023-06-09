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
 * WITHOUT WARRANTIES LEOR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MSTD_TUPLE_
#define MSTD_TUPLE_

/* <mstd_tuple>
 *
 * - includes toolchain's <tuple>
 * - For all toolchains, C++17 backports:
 *   - mstd::apply
 *   - mstd::make_from_tuple
 */

#include <tuple>

#if __cpp_lib_apply < 201603 || __cpp_lib_make_from_tuple < 201606
#include "mstd_utility.h" // integer_sequence
#endif
#if __cpp_lib_apply < 201603
#include <mstd_functional> // invoke
#endif

namespace mstd {
using std::tuple;
using std::ignore;
using std::make_tuple;
using std::forward_as_tuple;
using std::tie;
using std::tuple_cat;
using std::tuple_size;
using std::tuple_element;
using std::tuple_element_t;
using std::get;

// [tuple.apply]
#if __cpp_lib_apply >= 201603
using std::apply;
#else
namespace impl {
template <class F, class Tuple, size_t... I>
invoke_result_t<F, tuple_element_t<I, Tuple>...> apply(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return mstd::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
}
}

// apply<F, Tuple> - works also for tuple-like objects such as array or pair
// user-defined types can specialize std::get and std::tuple_size to make this work
template <class F, class Tuple>
auto apply(F&& f, Tuple&& t) ->
decltype(impl::apply(std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<tuple_size<remove_reference_t<Tuple>>::value>{}))
{
    return impl::apply(std::forward<F>(f), std::forward<Tuple>(t),
                       std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}
#endif

#if __cpp_lib_make_from_tuple >= 201606
using std::make_from_tuple;
#else
namespace impl {
template <class T, class Tuple, size_t... I>
T make_from_tuple(Tuple&& t, std::index_sequence<I...>)
{
    return T(std::get<I>(std::forward<Tuple>(t))...);
}
}

template <class T, class Tuple>
T make_from_tuple(Tuple&& t)
{
    return impl::make_from_tuple<T>(std::forward<Tuple>(t),
                       std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}
#endif

} // namespace mstd

#endif // MSTD_TUPLE_
