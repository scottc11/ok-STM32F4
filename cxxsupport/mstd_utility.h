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
#ifndef MSTD_UTILITY_
#define MSTD_UTILITY_

/* "mstd_utility.h"
 *
 * - includes toolchain's <utility>
 * - For ARM C 5, standard C++11/14 features:
 *   - include <initializer_list>
 *   - std::move, std::forward, std::exchange
 *   - std::declval
 *   - std::integer_sequence
 *   - include <algorithm> to get default std::swap
 *   - mstd::swap - substitute for std::swap that can move
 *   - std::swap(array)
 * - Swap assistance, to ensure moves happen on ARM C 5
 *   - mstd::swap - alias for std::swap, or a local moving implementation for ARM C 5
 * - For all toolchains, C++17/20 backports:
 *   - mstd::as_const
 */

#include <utility>

namespace mstd {
namespace rel_ops { using namespace std::rel_ops; }
using std::initializer_list;
using std::exchange;
using std::forward;
using std::move;
// No exceptions in mbed OS
template <typename T>
T &&move_if_noexcept(T &t) noexcept
{
    return mstd::move(t);
}
using std::declval;
using std::make_pair;
using std::get;
using std::pair;
using std::integer_sequence;
using std::index_sequence;
using std::make_integer_sequence;
using std::make_index_sequence;
using std::index_sequence_for;
using std::swap;
// C++17 [utility.as_const] */
#if __cpp_lib_as_const >= 201510
using std::as_const;
#else
template <typename _TypeT>
constexpr std::add_const_t<_TypeT> &as_const(_TypeT &__t) noexcept
{
    return __t;
}

template <typename _TypeT>
void as_const(_TypeT &&) = delete;
#endif

} // namespace mstd

#endif // MSTD_UTILITY_
