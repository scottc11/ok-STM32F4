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
#ifndef MSTD_MUTEX_
#define MSTD_MUTEX_

/* <mstd_mutex>
 *
 * - includes toolchain's <mutex> (if any)
 * - For toolchains not providing them, local implementation of C++11/14 equivalent features:
 *   - mstd::defer_lock etc
 *   - mstd::lock_guard
 *   - mstd::unique_lock
 *   - mstd::lock
 *   - mstd::try_lock
 * - If not available, local version of:
 *   - mstd::scoped_lock (C++17)
 * - For all toolchains, local implementations:
 *   - mstd::call_once, mstd::once_flag
 *   - mstd::mutex, mstd::recursive_mutex
 *
 * Toolchains will vary greatly in how much is in namespace std, depending on retargetting.
 */

#if !defined __IAR_SYSTEMS_ICC__
#include <mutex>
#endif

#if MBED_CONF_RTOS_PRESENT
#include "platform/SingletonPtr.h"
#include "rtos/Mutex.h"
#endif

#include "mstd_utility.h"
#include <mstd_functional>
#include <mstd_tuple>
#include <chrono>

#include "mbed_atomic.h"
#include "mbed_assert.h"

extern "C" int __cxa_guard_acquire(int *guard_object_p);
extern "C" void __cxa_guard_release(int *guard_object_p);

// IAR does not provide <mutex> at all - it errors on inclusion
// ARMC6 provides it, but it is empty unless _ARM_LIBCPP_EXTERNAL_THREADS is defined
// GCC has it, and only the actual `mutex` types are conditional on _GLIBCXX_HAS_GTHREADS
// So pick up std stuff, unless ICC, or ARMC6-without-threads
namespace mstd {
#if !defined __IAR_SYSTEMS_ICC__ && !defined _LIBCPP_HAS_NO_THREADS
using std::defer_lock;
using std::defer_lock_t;
using std::try_to_lock;
using std::try_to_lock_t;
using std::adopt_lock;
using std::adopt_lock_t;

using std::lock_guard;
using std::unique_lock;
using std::try_lock;
using std::lock;
#else
// [thread.lock]
struct defer_lock_t { };
struct try_to_lock_t { };
struct adopt_lock_t { };
constexpr defer_lock_t defer_lock;
constexpr try_to_lock_t try_to_lock;
constexpr adopt_lock_t adopt_lock;

// [thread.lock.guard]
template <class Mutex>
class lock_guard {
    Mutex &pm;
public:
    using mutex_type = Mutex;
    explicit lock_guard(Mutex &m) : pm(m) { m.lock(); }
    lock_guard(Mutex &m, adopt_lock_t) noexcept : pm(m) { }
    ~lock_guard() { pm.unlock(); }

    lock_guard(const lock_guard &) = delete;
    lock_guard &operator=(const lock_guard &) = delete;
};


// [thread.lock.unique]
template<class Mutex>
class unique_lock {
public:
    using mutex_type = Mutex;

    unique_lock() noexcept : pm(nullptr), owns(false) { }
    explicit unique_lock(mutex_type &m) : pm(&m), owns(true) { m.lock(); }
    unique_lock(mutex_type &m, defer_lock_t) noexcept : pm(&m), owns(false) { }
    unique_lock(mutex_type &m, try_to_lock_t) : pm(&m), owns(m.try_lock()) { }
    unique_lock(mutex_type &m, adopt_lock_t) : pm(&m), owns(true)  { }
    template <class Clock, class Duration>
    unique_lock(mutex_type &m, const std::chrono::time_point<Clock, Duration> &abs_time) : pm(&m), owns(m.try_lock_until(abs_time)) { }
    template <class Rep, class Period>
    unique_lock(mutex_type &m, const std::chrono::duration<Rep, Period> &rel_time) : pm(&m), owns(m.try_lock_for(rel_time)) { }
    ~unique_lock() { if (owns) pm->unlock(); }

    unique_lock(const unique_lock &) = delete;
    unique_lock &operator=(const unique_lock &) = delete;

    unique_lock(unique_lock &&u) noexcept : pm(u.pm), owns(u.owns) {
        u.pm = nullptr;
        u.owns = false;
    }

    unique_lock &operator=(unique_lock &&u) noexcept {
        if (owns) {
            pm->unlock();
        }
        pm = mstd::exchange(u.pm, nullptr);
        owns = mstd::exchange(u.owns, false);
        return *this;
    }

    void lock() {
        MBED_ASSERT(!owns);
        pm->lock();
        owns = true;
    }

    bool try_lock() {
        MBED_ASSERT(!owns);
        return owns = pm->try_lock();
    }

    template <class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration> &abs_time) {
        MBED_ASSERT(!owns);
        return owns = pm->try_lock_until(abs_time);
    }

    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period> &rel_time) {
        MBED_ASSERT(!owns);
        return owns = pm->try_lock_for(rel_time);
    }

    void unlock() {
        MBED_ASSERT(owns);
        pm->unlock();
        owns = false;
    }

    void swap(unique_lock &u) noexcept {
        mstd::swap(pm, u.pm);
        mstd::swap(owns, u.owns);
    }

    mutex_type *release() noexcept {
        owns = false;
        return mstd::exchange(pm, nullptr);
    }

    bool owns_lock() const noexcept {
        return owns;
    }

    explicit operator bool() const noexcept {
        return owns;
    }

    mutex_type *mutex() const noexcept {
        return pm;
    }

private:
    mutex_type *pm;
    bool owns;
};

template<class Mutex>
void swap(unique_lock<Mutex> &x, unique_lock<Mutex> &y) noexcept
{
    x.swap(y);
}

// [thread.lock.algorithm]
template <class L1, class L2>
int try_lock(L1 &l1, L2 &l2)
{
    unique_lock<L1> u1(l1, try_to_lock);
    if (!u1) {
        return 0;
    }
    if (l2.try_lock()) {
        u1.release();
        return -1;
    } else {
        return 1;
    }
}

template <class L1, class L2, class L3, class... LN>
int try_lock(L1 &l1, L2 &l2, L3 &l3, LN &... ln)
{
    unique_lock<L1> u1(l1, try_to_lock);
    if (!u1) {
        return 0;
    }
    int result = mstd::try_lock(l2, l3, ln...);
    if (result == -1) {
        u1.release(); // make u1 release l1 so it remains locked when we return
        return -1;
    } else {
        return result + 1; // u1 unlocks l1 when we return
    }
}

// Howard Hinnant's "smart" algorithm from
// http://howardhinnant.github.io/dining_philosophers.html
//
// 1) Lock a mutex
// 2) Try-lock all the rest
// 3) If try-lock fails, retry, but starting with the mutex whose try-lock failed
//    (so we expect to block on that lock)
//
// Do not bother with the "polite" yield, as it adds an OS dependency and we
// want to optimise for space, not speed.
// Use of unique_lock is necessary to make the code correct in case of exceptions;
// we don't strictly require this, but stick with the RAII form nevertheless -
// overhead of unique_lock should be minimal with optimisation enabled.
template <class L1, class L2>
void lock(L1 &l1, L2 &l2)
{
    for (;;) {
        {
            unique_lock<L1> u1(l1);
            if (l2.try_lock()) {
                u1.release(); // make u1 release l1 so it remains locked when we return
                return;
            }
        } // u1 unlocks l1 when we leave scope
        {
            unique_lock<L2> u2(l2);
            if (l1.try_lock()) {
                u2.release();
                return;
            }
        } // u2 unlocks l2 when we leave scope
    }
}

namespace impl {
template <class L1, class L2, class L3, class... LN>
void lock_from(int first, L1 &l1, L2 &l2, L3 &l3, LN &... ln)
{
    for (;;) {
        switch (first) {
        case 1:
            {
                unique_lock<L1> u1(l1);
                first = mstd::try_lock(l2, l3, ln...);
                if (first == -1) {
                    u1.release();
                    return;
                }
            }
            first += 2;
            break;
        case 2:
            {
                unique_lock<L2> u2(l2);
                first = mstd::try_lock(l3, ln..., l1);
                if (first == -1) {
                    u2.release();
                    return;
                }
            }
            first += 3;
            if (first > 3 + sizeof...(LN)) {
                first = 1;
            }
            break;
        default:
            return impl::lock_from(first - 2, l3, ln..., l1, l2);
        }
    }
}
}

template <class L1, class L2, class L3, class... LN>
void lock(L1 &l1, L2 &l2, L3 &l3, LN &... ln)
{
    impl::lock_from(1, l1, l2, l3, ln...);
}

#endif


#if __cpp_lib_scoped_lock >= 201703
using std::scoped_lock;
#else
// [thread.lock.scoped]
// 2+ locks - use std::lock
template <class... MutexTypes>
class scoped_lock {
    mstd::tuple<MutexTypes &...> pm;
    static void ignore(...) { }
public:
    explicit scoped_lock(MutexTypes &... m) : pm(tie(m...)) { mstd::lock(m...); }
    explicit scoped_lock(adopt_lock_t, MutexTypes &... m) noexcept : pm(mstd::tie(m...)) { }
    ~scoped_lock() { mstd::apply([](MutexTypes &... m) { ignore( (void(m.unlock()),0) ...); }, pm); }

    scoped_lock(const scoped_lock &) = delete;
    scoped_lock &operator=(const scoped_lock &) = delete;
};

// 0 locks - no-op
template <>
class scoped_lock<> {
public:
    explicit scoped_lock() = default;
    explicit scoped_lock(adopt_lock_t) noexcept { }
    ~scoped_lock() = default;

    scoped_lock(const scoped_lock &) = delete;
    scoped_lock &operator=(const scoped_lock &) = delete;
};

// 1 lock - simple lock, equivalent to lock_guard<Mutex>
template <class Mutex>
class scoped_lock<Mutex> {
    Mutex &pm;
public:
    using mutex_type = Mutex;
    explicit scoped_lock(Mutex &m) : pm(m) { m.lock(); }
    explicit scoped_lock(adopt_lock_t, Mutex &m) noexcept : pm(m) { }
    ~scoped_lock() { pm.unlock(); }

    scoped_lock(const scoped_lock &) = delete;
    scoped_lock &operator=(const scoped_lock &) = delete;
};
#endif

// [thread.once.onceflag]
// Always local implementation - need to investigate GCC + ARMC6 retargetting
struct once_flag {
    constexpr once_flag() noexcept : __guard() { }
    once_flag(const once_flag &) = delete;
    once_flag &operator=(const once_flag &) = delete;
    ~once_flag() = default;
private:
    template <class Callable, class... Args>
    friend void call_once(once_flag &flag, Callable&& f, Args&&... args);
    int __guard;
};

// [thread.once.callonce]
template <class Callable, class... Args>
void call_once(once_flag &flag, Callable&& f, Args&&... args)
{
    if (!(core_util_atomic_load_explicit((uint8_t *)&flag.__guard, mbed_memory_order_acquire) & 1)) {
        if (__cxa_guard_acquire(&flag.__guard)) {
             mstd::invoke(mstd::forward<Callable>(f), mstd::forward<Args>(args)...);
             __cxa_guard_release(&flag.__guard);
        }
    }
}

// [thread.mutex.class]
// Always local implementation - need to investigate GCC + ARMC6 retargetting
#if MBED_CONF_RTOS_PRESENT
class _Mutex_base {
    // Constructor must be constexpr - we are required to initialise on first use
    // not in our constructor. (So that mutex use in static constructors is safe).
    SingletonPtr<rtos::Mutex> _pm;
public:
    constexpr _Mutex_base() noexcept = default;
    ~_Mutex_base();
    _Mutex_base(const _Mutex_base &) = delete;
    _Mutex_base &operator=(const _Mutex_base &) = delete;
    void lock();
    bool try_lock();
    void unlock();
};
#else
class _Mutex_base {
public:
    constexpr _Mutex_base() noexcept = default;
    ~_Mutex_base() = default;
    _Mutex_base(const _Mutex_base &) = delete;
    _Mutex_base &operator=(const _Mutex_base &) = delete;
    void lock() { }
    bool try_lock() { return true; }
    void unlock() { }
};
#endif

// We don't currently distinguish implementations (and aren't required to -
// current thread not owning a non-recursive one is a precondition, we don't
// have to take any special action).
class mutex : public _Mutex_base {
};

// [thread.mutex.recursive]
class recursive_mutex : public _Mutex_base {
};

} // namespace mstd

#endif // MSTD_MUTEX_
