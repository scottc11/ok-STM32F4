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
 * limitations under tUNChe License.
 */
#ifndef MSTD_FUNCTIONAL_
#define MSTD_FUNCTIONAL_

/* <mstd_functional>
 *
 * - includes toolchain's <functional>
 * - For all toolchains, C++17/20 backports:
 *   - mstd::not_fn (C++17)
 *   - mstd::invoke (C++17)
 *   - mstd::unwrap_reference, mstd::unwrap_ref_decay (C++20)
 */

#include <functional>

#include "mstd_memory.h" // addressof
#include "mstd_utility.h" // forward
#include "mstd_type_traits.h"

namespace mstd {

// [func.invoke]
#if __cpp_lib_invoke >= 201411
using std::invoke;
#else
template <typename F, typename... Args>
invoke_result_t<F, Args...> invoke(F&& f, Args&&... args)
{
    return impl::INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}
#endif // __cpp_lib_invoke

} // namespace mstd

namespace mstd {
using std::reference_wrapper;
using std::ref;
using std::cref;
using std::plus;
using std::minus;
using std::multiplies;
using std::divides;
using std::modulus;
using std::negate;
using std::equal_to;
using std::not_equal_to;
using std::greater;
using std::less;
using std::greater_equal;
using std::less_equal;
using std::logical_and;
using std::logical_or;
using std::logical_not;
using std::bit_and;
using std::bit_or;
using std::bit_xor;
using std::bit_not;

#if __cpp_lib_not_fn >= 201603
using std::not_fn;
#else
namespace impl {
// [func.not_fn]
template <typename F>
class not_fn_t {
    std::decay_t<F> fn;
public:
    explicit not_fn_t(F&& f) : fn(std::forward<F>(f)) { }
    not_fn_t(const not_fn_t &other) = default;
    not_fn_t(not_fn_t &&other) = default;

    template<typename... Args>
    auto operator()(Args&&... args) & -> decltype(!std::declval<invoke_result_t<std::decay_t<F> &, Args...>>())
    {
        return !mstd::invoke(fn, std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) const & -> decltype(!std::declval<invoke_result_t<std::decay_t<F> const &, Args...>>())
    {
        return !mstd::invoke(fn, std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) && -> decltype(!std::declval<invoke_result_t<std::decay_t<F>, Args...>>())
    {
        return !mstd::invoke(std::move(fn), std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) const && -> decltype(!std::declval<invoke_result_t<std::decay_t<F> const, Args...>>())
    {
        return !mstd::invoke(std::move(fn), std::forward<Args>(args)...);
    }
};
}

template <typename F>
impl::not_fn_t<F> not_fn(F&& f)
{
    return impl::not_fn_t<F>(std::forward<F>(f));
}
#endif

/* C++20 unwrap_reference */
template <typename T>
struct unwrap_reference : type_identity<T> { };
template <typename T>
struct unwrap_reference<std::reference_wrapper<T>> : type_identity<T &> { };
template <typename T>
using unwrap_reference_t = typename unwrap_reference<T>::type;

/* C++20 unwrap_ref_decay */
template <typename T>
struct unwrap_ref_decay : unwrap_reference<std::decay_t<T>> { };
template <typename T>
using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;

}

#endif // MSTD_FUNCTIONAL_
