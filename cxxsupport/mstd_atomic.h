/*
 * Copyright (c) 2017 ARM Limited
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

#ifndef MSTD_ATOMIC_
#define MSTD_ATOMIC_

#include <stddef.h>
#include <stdint.h>
#include "mstd_memory.h"
#include "mstd_type_traits.h"
#include "mstd_utility.h"
#include "platform/mbed_atomic.h"
#include "platform/mbed_critical.h"
#include "platform/CriticalSectionLock.h"

/*
 * mstd::atomic template and types are designed to be as close as possible to C++11
 * std::atomic. Key differences:
 *
 * - Operations are specified as atomic with respect to interrupts as well as
 *   threads
 * - "Lock-free" indicates that a critical section is used, otherwise
 *   exclusive accesses.
 * - Default initialization follows C17 and proposed C++2x rules - ie that
 *   like normal objects they are zero-initialized if static or thread-local,
 *   else in an indeterminate state when automatic. There is no ATOMIC_VAR_INIT()
 *   equivalent.
 */

#ifndef MBED_EXCLUSIVE_ACCESS
#define MSTD_ATOMIC_BOOL_LOCK_FREE      0
#define MSTD_ATOMIC_CHAR_LOCK_FREE      0
#define MSTD_ATOMIC_CHAR16_T_LOCK_FREE  0
#define MSTD_ATOMIC_CHAR32_T_LOCK_FREE  0
#define MSTD_ATOMIC_WCHAR_T_LOCK_FREE   0
#define MSTD_ATOMIC_SHORT_LOCK_FREE     0
#define MSTD_ATOMIC_INT_LOCK_FREE       0
#define MSTD_ATOMIC_LONG_LOCK_FREE      0
#define MSTD_ATOMIC_LLONG_LOCK_FREE     0
#define MSTD_ATOMIC_POINTER_LOCK_FREE   0
#else
#define MSTD_ATOMIC_BOOL_LOCK_FREE      2
#define MSTD_ATOMIC_CHAR_LOCK_FREE      2
#define MSTD_ATOMIC_CHAR16_T_LOCK_FREE  2
#define MSTD_ATOMIC_CHAR32_T_LOCK_FREE  2
#define MSTD_ATOMIC_WCHAR_T_LOCK_FREE   2
#define MSTD_ATOMIC_SHORT_LOCK_FREE     2
#define MSTD_ATOMIC_INT_LOCK_FREE       2
#define MSTD_ATOMIC_LONG_LOCK_FREE      2
#define MSTD_ATOMIC_LLONG_LOCK_FREE     0
#define MSTD_ATOMIC_POINTER_LOCK_FREE   2
#endif

namespace mstd {

/** Atomic template
 *
 * `mstd::atomic<T>` is intended to work as per C++14 `std::atomic<T>`. `T` must be a
 * _TriviallyCopyable_, _CopyConstructible_ and _CopyAssignable_ type.
 * - All standard methods of `std::atomic` are supplied:
 *   + For any `T`: `load`, `store`, `exchange`, `compare_exchange_weak`,
 *     `compare_exchange_strong`, `operator T`, `operator=(T)`, `Atomic(T)`;
 *   + For integers and pointers: `++`, `+=`, `--`, `-=`, `fetch_add`, `fetch_sub`;
 *   + For integers: `&=`, `|=`, `^=`, `fetch_and`, `fetch_or`, `fetch_xor`.
 * - Operations are guaranteed atomic with respect to interrupts, and the
 *   operations can be used in interrupts - `std::atomic` implementations don't specify this.
 * - Implementation is optimized for uniprocessor use (no DMB instructions),
 *   unlike typical `std::atomic` implementations.
 * - Lock-free versions (LDREX/STREX) are used for user types if small enough and available,
 *   otherwise critical sections are used.
 * - If used with large objects, interrupt latency may be impacted.
 * - Valid initialisation forms are:
 *   + `atomic<int> foo;` (zero initialized if static or thread-local, else value indeterminate)
 *   + `atomic_init(&foo, 2);` (initialize a default-initialized variable, once only, not atomic)
 *   + `atomic<int> foo(2);` (value initialization)
 *   + `atomic<int> foo = { 2 };` (also legal C11 with _Atomic int)
 *   + `atomic<int> foo = 2;` (C++17 or later only - also legal C11 with _Atomic int)
 *   Note that the lack of a copy constructor limits the simple-looking assignment initialization
 *   to C++17 or later only.
 * - The value constructor is not available for small custom types.
 * - `MSTD_ATOMIC_XXX_LOCK_FREE` replaces `ATOMIC_XXX_LOCK_FREE` - "locking" forms
 *   take a critical section, non-locking do not.
 * - For `bool`, integer types and pointers, storage is compatible with the
 *   plain types. If necessary, they can be substituted as plain types for C
 *   compatibility in headers, and accessed using core_util_atomic functions.
 *   @code
 *       struct foo {
 *       #ifdef __cplusplus
 *          mstd::atomic_uint32_t counter; // Use C++ templates from C++ code
 *       #else
 *          uint32_t counter;  // Could use core_util_atomic_xxx_u32 from C code, or just have this for structure layout.
 *       #endif
 *       };
 *   @endcode
 */
template<typename T>
class atomic;

/* Pull C enum from mbed_critical.h into mstd namespace */
using memory_order = ::mbed_memory_order;
constexpr memory_order memory_order_relaxed = mbed_memory_order_relaxed;
constexpr memory_order memory_order_consume = mbed_memory_order_consume;
constexpr memory_order memory_order_acquire = mbed_memory_order_acquire;
constexpr memory_order memory_order_release = mbed_memory_order_release;
constexpr memory_order memory_order_acq_rel = mbed_memory_order_acq_rel;
constexpr memory_order memory_order_seq_cst = mbed_memory_order_seq_cst;

namespace impl {

/* For types up to uint64_t size, we use the mbed_critical.h functions with
 * uintX_t containers. Otherwise, we do it ourselves, with no special alignment.
 */
// *INDENT-OFF*
template<typename T>
using atomic_container = conditional  <sizeof(T) <= sizeof(uint8_t),  uint8_t,
                         conditional_t<sizeof(T) <= sizeof(uint16_t), uint16_t,
                         conditional_t<sizeof(T) <= sizeof(uint32_t), uint32_t,
                         conditional_t<sizeof(T) <= sizeof(uint64_t), uint64_t,
                                                                      T
                                      >>>>;
// *INDENT-ON*

template<typename T>
using atomic_container_t = typename atomic_container<T>::type;

template<typename N>
struct atomic_container_is_lock_free;

template<>
struct atomic_container_is_lock_free<uint8_t> : bool_constant<bool(MSTD_ATOMIC_CHAR_LOCK_FREE)> { };

template<>
struct atomic_container_is_lock_free<uint16_t> : bool_constant<bool(MSTD_ATOMIC_SHORT_LOCK_FREE)> { };

template<>
struct atomic_container_is_lock_free<uint32_t> : bool_constant<bool(MSTD_ATOMIC_INT_LOCK_FREE)> { };

template<>
struct atomic_container_is_lock_free<uint64_t> : bool_constant<bool(MSTD_ATOMIC_LLONG_LOCK_FREE)> { };

template<typename T>
using atomic_is_lock_free = atomic_container_is_lock_free<atomic_container_t<T>>;

/* If one order is given for compare_exchange, it's reduced for failure case that doesn't store */
MBED_FORCEINLINE constexpr memory_order memorder_for_failure(memory_order order)
{
    return order == memory_order_acq_rel ? memory_order_acquire :
           order == memory_order_release ? memory_order_relaxed : order;
}

/* Base template for a raw Atomic (arbitrary type T), using atomic storage size N.
 * This generic implementation uses critical sections and has no alignment requirements.
 * There are specialisations for smaller sizes below.
 */
template<typename T, typename = void>
struct AtomicBaseRaw {
    using value_type = T;
    AtomicBaseRaw() noexcept = default;
    constexpr AtomicBaseRaw(T v) noexcept : data(std::move(v))
    {
    }
    bool is_lock_free() const volatile noexcept
    {
        return false;
    }
    T load(memory_order order = memory_order_seq_cst) const volatile noexcept
    {
        MBED_CHECK_LOAD_ORDER(order);
        // Cope with T not having default constructor
        union {
            char c[sizeof(T)];
            T val;
        } ret;
        {
            mbed::CriticalSectionLock lock;
            memcpy(std::addressof(ret.val), const_cast<const T *>(std::addressof(data)), sizeof(T));
        }
        return std::move(ret.val);
    }
    T load(memory_order order = memory_order_seq_cst) const noexcept
    {
        MBED_CHECK_LOAD_ORDER(order);
        mbed::CriticalSectionLock lock;
        return data;
    }
    void store(T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        MBED_CHECK_STORE_ORDER(order);
        mbed::CriticalSectionLock lock;
        memcpy(const_cast<T *>(std::addressof(data)), std::addressof(desired), sizeof(T));
    }
    void store(T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        MBED_CHECK_STORE_ORDER(order);
        mbed::CriticalSectionLock lock;
        data = std::move(desired); // MoveAssignable
    }
    T exchange(T desired, memory_order = memory_order_seq_cst) volatile noexcept
    {
        // Cope with T not having default constructor
        union {
            char c[sizeof(T)];
            T val;
        } old;
        {
            mbed::CriticalSectionLock lock;
            memcpy(std::addressof(old.val), const_cast<const T *>(std::addressof(data)), sizeof(T));
            memcpy(const_cast<T *>(std::addressof(data)), std::addressof(desired), sizeof(T));
        }
        return old.val;
    }
    T exchange(T desired, memory_order = memory_order_seq_cst) noexcept
    {
        mbed::CriticalSectionLock lock;
        T old = std::move(data); // MoveConstructible
        data = std::move(desired); // MoveAssignable
        return old;
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        MBED_CHECK_CAS_ORDER(success, failure);
        mbed::CriticalSectionLock lock;
        if (memcmp(const_cast<const T *>(std::addressof(data)), std::addressof(expected), sizeof(T)) == 0) {
            memcpy(const_cast<T *>(std::addressof(data)), std::addressof(desired), sizeof(T));
            return true;
        } else {
            memcpy(std::addressof(expected), const_cast<const T *>(std::addressof(data)), sizeof(T));
            return false;
        }
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order success, memory_order failure) noexcept
    {
        MBED_CHECK_CAS_ORDER(success, failure);
        mbed::CriticalSectionLock lock;
        if (memcmp(std::addressof(data), std::addressof(expected), sizeof(T)) == 0) {
            data = std::move(desired); // MoveAssignable
            return true;
        } else {
            expected = data; // CopyAssignable
            return false;
        }
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        return compare_exchange_strong(expected, desired, success, failure);
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order success, memory_order failure) noexcept
    {
        return compare_exchange_strong(expected, desired, success, failure);
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        return compare_exchange_strong(expected, desired, order, memorder_for_failure(order));
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        return compare_exchange_strong(expected, desired, order, memorder_for_failure(order));
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order order = memory_order_seq_cst) volatile noexcept
    {
        return compare_exchange_weak(expected, desired, order, memorder_for_failure(order));
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order order = memory_order_seq_cst) noexcept
    {
        return compare_exchange_weak(expected, desired, order, memorder_for_failure(order));
    }
protected:
    union {
        // Having the union makes us just get zero-initialised, as per std::atomic, or our specializations,
        // rather than actually running T's default constructor.
        char dummy_for_zero_init;
        T data;
    };
    void init(T desired) volatile noexcept
    {
        memcpy(const_cast<T *>(std::addressof(data)), std::addressof(desired), sizeof(T));
    }
    void init(T desired) noexcept
    {
        data = std::move(desired);
    }
};

template<typename T, typename A, int N>
struct AtomicSmallStoragePadded {
    union {
        A u;
        struct {
            T data;
            char pad[N];
        };
    };
    AtomicSmallStoragePadded() noexcept = default;
    constexpr AtomicSmallStoragePadded(T v) noexcept : data(std::move(v)), pad{0}
    {
    }
    constexpr AtomicSmallStoragePadded(A v) noexcept : u(v)
    {
    }
};

template<typename T, typename A>
struct AtomicSmallStorageUnpadded {
    union {
        A u;
        T data;
    };
    AtomicSmallStorageUnpadded() noexcept = default;
    constexpr AtomicSmallStorageUnpadded(T v) noexcept : data(std::move(v))
    {
    }
    constexpr AtomicSmallStorageUnpadded(A v) noexcept : u(v)
    {
    }
};

// *INDENT-OFF*
template<typename T, typename Storage = atomic_container_t<T>>
using AtomicSmallStorage = std::conditional_t<sizeof(Storage) == sizeof(T),
                AtomicSmallStorageUnpadded<T, Storage>,
                AtomicSmallStoragePadded<T, Storage, sizeof(Storage) - sizeof(T)>>;
// *INDENT-ON*

/* Base implementation specialisation for arbitrary small type T of size N,
 * using corresponding atomic_xxx functions acting on uintX_t data type A of that size.
 * This does involve type punning on a union, so isn't strictly legal, but it's no worse than
 * what has always been done with the pointer atomics.
 * Always pad when necessary so that compare-exchange works.
 *
 * It's only worth using the specific small form if there is a real lock-free implementation.
 * Otherwise the overhead of shuffling in and out of the integer container is larger than just
 * doing it directly.
 */
template<typename T>
class AtomicBaseRaw<T, std::enable_if_t<atomic_is_lock_free<T>::value>> {
    AtomicSmallStorage<T> storage;
public:
    using value_type = T;
    AtomicBaseRaw() noexcept = default;
    constexpr AtomicBaseRaw(T v) : storage(std::move(v)) { }
    bool is_lock_free() const volatile noexcept
    {
        return atomic_is_lock_free<T>::value;
    }
    T load() const volatile noexcept
    {
        AtomicSmallStorage<T> loaded(core_util_atomic_load(&storage.u));
        return loaded.data;
    }
    T load(memory_order order) const volatile noexcept
    {
        AtomicSmallStorage<T> loaded(core_util_atomic_load_explicit(&storage.u, order));
        return loaded.data;
    }
    T load() const noexcept
    {
        AtomicSmallStorage<T> loaded(core_util_atomic_load(&storage.u));
        return loaded.data;
    }
    T load(memory_order order) const noexcept
    {
        AtomicSmallStorage<T> loaded(core_util_atomic_load_explicit(&storage.u, order));
        return loaded.data;
    }
    void store(T desired) volatile noexcept
    {
        AtomicSmallStorage<T> tostore(desired);
        core_util_atomic_store(&storage.u, tostore.u);
    }
    void store(T desired, memory_order order) volatile noexcept
    {
        AtomicSmallStorage<T> tostore(desired);
        core_util_atomic_store_explicit(&storage.u, tostore.u, order);
    }
    void store(T desired) noexcept
    {
        AtomicSmallStorage<T> tostore(desired);
        core_util_atomic_store(&storage.u, tostore.u);
    }
    void store(T desired, memory_order order) noexcept
    {
        AtomicSmallStorage<T> tostore(desired);
        core_util_atomic_store_explicit(&storage.u, tostore.u, order);
    }
    T exchange(T desired, memory_order = memory_order_seq_cst) volatile noexcept
    {
        AtomicSmallStorage<T> exchanged(desired);
        exchanged.u = core_util_atomic_exchange(&storage.u, exchanged.u);
        return exchanged.data;
    }
    bool compare_exchange_strong(T &expected, T desired) volatile noexcept
    {
        AtomicSmallStorage<T> expcur(expected);
        AtomicSmallStorage<T> tostore(desired);
        bool result = core_util_atomic_compare_exchange_strong(&storage.u, &expcur.u, tostore.u);
        if (!result) {
            expected = expcur.data;
        }
        return result;
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        AtomicSmallStorage<T> expcur(expected);
        AtomicSmallStorage<T> tostore(desired);
        bool result = core_util_atomic_compare_exchange_strong_explicit(&storage.u, &expcur.u, tostore.u, success, failure);
        if (!result) {
            expected = expcur.data;
        }
        return result;
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order order) volatile noexcept
    {
        return compare_exchange_strong(expected, desired, order, memorder_for_failure(order));
    }
    bool compare_exchange_weak(T &expected, T desired) volatile noexcept
    {
        AtomicSmallStorage<T> expcur(expected);
        AtomicSmallStorage<T> tostore(desired);
        bool result = core_util_atomic_compare_exchange_weak(&storage.u, &expcur.u, tostore.u);
        if (!result) {
            expected = expcur.data;
        }
        return result;
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        AtomicSmallStorage<T> expcur(expected);
        AtomicSmallStorage<T> tostore(desired);
        bool result = core_util_atomic_compare_exchange_weak_explicit(&storage.u, &expcur.u, tostore.u, success, failure);
        if (!result) {
            expected = expcur.data;
        }
        return result;
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order order) volatile noexcept
    {
        return compare_exchange_weak(expected, desired, order, memorder_for_failure(order));
    }
protected:
    void init(T desired) volatile noexcept
    {
        AtomicSmallStorage<T> tostore(desired);
        memcpy(const_cast<decltype(tostore.u) *>(&storage.u), &tostore.u, sizeof storage.u);
    }
    void init(T desired) noexcept
    {
        AtomicSmallStorage<T> tostore(desired);
        storage.u = std::move(tostore.u);
    }
};

/* Template for an integer or pointer Atomic of type T, using atomic storage A
 * Same functionality as AtomicBaseRaw, but can use simpler implementation using casts
 * to convert between type T and the underlying storage type. Doesn't change functionality,
 * compilers can generally optimise this better (will particularly help in debug build).
 * C casts must be used as reinterpret_cast can't handle integer<->integer, and static_cast
 * can't handle pointer<->integer. Note that
 * we always have A be unsigned, so that our arithmetic is unsigned and defined on overflow.
 * Compilers can and do treat signed arithmetic overflow as undefined, but not cast overflow.
 * (C++20 explicitly defines assignment of unsigned to signed as 2's-complement).
 * Our data field is of type T, not A, to permit value/aggregate initialisation.
 */
template<typename T, typename A = atomic_container_t<T>>
struct AtomicBaseInt {
    static_assert(sizeof(T) == sizeof(A), "AtomicBaseInt size mismatch");
    using value_type = T;
    AtomicBaseInt() noexcept = default;
    constexpr AtomicBaseInt(T v) noexcept : u(A(v))
    {
    }
    bool is_lock_free() const volatile noexcept
    {
        return atomic_container_is_lock_free<A>::value;
    }
    T load() const volatile noexcept
    {
        return T(core_util_atomic_load(&u));
    }
    T load(memory_order order) const volatile noexcept
    {
        return T(core_util_atomic_load_explicit(&u, order));
    }
    T load() const noexcept
    {
        return T(core_util_atomic_load(&u));
    }
    T load(memory_order order) const noexcept
    {
        return T(core_util_atomic_load_explicit(&u, order));
    }
    void store(T desired) volatile noexcept
    {
        core_util_atomic_store(&u, A(desired));
    }
    void store(T desired, memory_order order) volatile noexcept
    {
        core_util_atomic_store_explicit(&u, A(desired), order);
    }
    void store(T desired) noexcept
    {
        core_util_atomic_store(&u, A(desired));
    }
    void store(T desired, memory_order order) noexcept
    {
        core_util_atomic_store_explicit(&u, A(desired), order);
    }
    T exchange(T desired) volatile noexcept
    {
        A d = A(desired);
        return T(core_util_atomic_exchange(&u, d));
    }
    T exchange(T desired, memory_order order) volatile noexcept
    {
        A d = A(desired);
        return T(core_util_atomic_exchange_explicit(&u, d, order));
    }
    bool compare_exchange_strong(T &expected, T desired) volatile noexcept
    {
        A *expcur = reinterpret_cast<A *>(&expected);
        return core_util_atomic_compare_exchange_strong(&u, expcur, A(desired));
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        A *expcur = reinterpret_cast<A *>(&expected);
        return core_util_atomic_compare_exchange_strong_explicit(&u, expcur, A(desired), success, failure);
    }
    bool compare_exchange_strong(T &expected, T desired, memory_order order) volatile noexcept
    {
        return compare_exchange_strong(expected, desired, order, memorder_for_failure(order));
    }
    bool compare_exchange_weak(T &expected, T desired) volatile noexcept
    {
        A *expcur = reinterpret_cast<A *>(&expected);
        return core_util_atomic_compare_exchange_weak(&u, expcur, A(desired));
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order success, memory_order failure) volatile noexcept
    {
        A *expcur = reinterpret_cast<A *>(&expected);
        return core_util_atomic_compare_exchange_weak_explicit(&u, expcur, A(desired), success, failure);
    }
    bool compare_exchange_weak(T &expected, T desired, memory_order order) volatile noexcept
    {
        return compare_exchange_weak(expected, desired, order, memorder_for_failure(order));
    }
protected:
    A u;
    void init(T desired) volatile noexcept
    {
        u = A(desired);
    }
    void init(T desired) noexcept
    {
        u = A(desired);
    }
};

/* Template for an integer or pointer Atomic, including increment and
 * decrement functionality. If StrideT is void, then the increment and
 * decrement operators are ill-formed, as desired for atomic<void *>.
 */
template<typename T, typename DiffT = T, typename StrideT = char, typename A = atomic_container_t<T>>
struct AtomicWithAdd : public AtomicBaseInt<T, A> {
    using difference_type = DiffT;
    using AtomicBaseInt<T, A>::AtomicBaseInt;

    T operator++() volatile noexcept
    {
        A d = static_cast<A>(sizeof(StrideT));
        return T(core_util_atomic_incr(&this->u, d));
    }
    T operator++(int) volatile noexcept
    {
        A d = static_cast<A>(sizeof(StrideT));
        return T(core_util_atomic_fetch_add(&this->u, d));
    }
    T operator--() volatile noexcept
    {
        A d = static_cast<A>(sizeof(StrideT));
        return T(core_util_atomic_decr(&this->u, d));
    }
    T operator--(int) volatile
    {
        A d = static_cast<A>(sizeof(StrideT));
        return T(core_util_atomic_fetch_sub(&this->u, d));
    }
    T fetch_add(DiffT arg) volatile
    {
        A d = static_cast<A>(arg * sizeof(StrideT));
        return T(core_util_atomic_fetch_add(&this->u, d));
    }
    T fetch_add(DiffT arg, memory_order order) volatile
    {
        A d = static_cast<A>(arg * sizeof(StrideT));
        return T(core_util_atomic_fetch_add_explicit(&this->u, d, order));
    }
    T operator+=(DiffT arg) volatile
    {
        A d = static_cast<A>(arg * sizeof(StrideT));
        return T(core_util_atomic_incr(&this->u, d));
    }
    T fetch_sub(DiffT arg) volatile
    {
        A d = static_cast<A>(arg * sizeof(StrideT));
        return T(core_util_atomic_fetch_sub(&this->u, d));
    }
    T fetch_sub(DiffT arg, memory_order order) volatile
    {
        A d = static_cast<A>(arg * sizeof(StrideT));
        return T(core_util_atomic_fetch_sub_explicit(&this->u, d, order));
    }
    T operator-=(DiffT arg) volatile
    {
        A d = static_cast<A>(arg * sizeof(StrideT));
        return T(core_util_atomic_decr(&this->u, d));
    }
};

/* Template for an integer Atomic with bitwise operations
 */
template<typename T, typename A = atomic_container_t<T>>
struct AtomicWithBitwise : public AtomicWithAdd<T, A> {
    using AtomicWithAdd<T, A>::AtomicWithAdd;

    T fetch_and(T arg) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_fetch_and(&this->u, d));
    }
    T fetch_and(T arg, memory_order order) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_fetch_and_explicit(&this->u, d, order));
    }
    T operator&=(T arg) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_and_fetch(&this->u, d));
    }
    T fetch_or(T arg) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_fetch_or(&this->u, d));
    }
    T fetch_or(T arg, memory_order order) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_fetch_or_explicit(&this->u, d, order));
    }
    T operator|=(T arg) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_or_fetch(&this->u, d));
    }
    T fetch_xor(T arg) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_fetch_xor(&this->u, d));
    }
    T fetch_xor(T arg, memory_order order) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_fetch_xor_explicit(&this->u, d, order));
    }
    T operator^=(T arg) volatile noexcept
    {
        A d = static_cast<A>(arg);
        return static_cast<T>(core_util_atomic_xor_fetch(&this->u, d));
    }
};

/* Selector between the implementations
 * bool -> AtomicBaseInt
 * other integral types -> AtomicWithBitwise
 * everything else -> AtomicBaseRaw
 * (Pointers are specialized in the public API)
 */
// *INDENT-OFF*
template<typename T, typename = void>
struct AtomicSelector : type_identity<AtomicBaseRaw<T>> { };

template<typename T>
struct AtomicSelector<T, enable_if_t<is_same<bool, T>::value>>
                        : type_identity<AtomicBaseInt<T>> { };

template<typename T>
struct AtomicSelector<T, enable_if_t<is_integral<T>::value && !is_same<bool, T>::value>>
                        : type_identity<AtomicWithBitwise<T>> { };
// *INDENT-ON*

template<typename T>
using Atomic = typename AtomicSelector<T>::type;

} // namespace impl

template<typename T>
void atomic_init(volatile atomic<T> *obj, typename atomic<T>::value_type desired) noexcept;

template<typename T>
void atomic_init(atomic<T> *obj, typename atomic<T>::value_type desired) noexcept;

/* Base template - let impl::Atomic<T> dispatch to raw, base integer or integer-with-bitwise */
template<typename T>
struct atomic : public impl::Atomic<T> {
    // Constraints from LWG 3012
    static_assert(is_trivially_copyable<T>::value, "Atomic types must be TriviallyCopyable");
    static_assert(is_copy_constructible<T>::value, "Atomic types must be CopyConstructible");
    static_assert(is_move_constructible<T>::value, "Atomic types must be MoveConstructible");
    static_assert(is_copy_assignable<T>::value, "Atomic types must be CopyAssignable");
    static_assert(is_move_assignable<T>::value, "Atomic types must be MoveAssignable");
    atomic() noexcept = default;
    atomic(const atomic &) = delete;
    constexpr atomic(T v) noexcept : impl::Atomic<T>(std::move(v))
    {
    }
    operator T() const volatile noexcept
    {
        return this->load();
    }
    operator T() const noexcept
    {
        return this->load();
    }
    T operator=(T desired) volatile noexcept
    {
        this->store(desired);
        return desired;
    }
    T operator=(T desired) noexcept
    {
        this->store(desired);
        return desired;
    }
    atomic &operator=(const atomic &) = delete;
private:
    friend void atomic_init<>(volatile atomic *obj, typename atomic::value_type desired) noexcept;
    friend void atomic_init<>(atomic *obj, typename atomic::value_type desired) noexcept;
};


/* Pointer specialisation - support increment and decrement by ptrdiff_t,
 * as long as sizeof(T) is valid to act as the stride.  Annoyingly, C++11
 * doesn't provide operator->, so neither do we, so you have to say
 * "aptr.load()->member" to use it to access a structure. *aptr is fine though.
 */
template<typename T>
struct atomic<T *> : public impl::AtomicWithAdd<T *, ptrdiff_t, T> {
    atomic() noexcept = default;
    atomic(const atomic &) = delete;
    constexpr atomic(T *v) noexcept : impl::AtomicWithAdd<T *, ptrdiff_t, T>(v)
    {
    }
    operator T *() const volatile noexcept
    {
        return this->load();
    }
    operator T *() const noexcept
    {
        return this->load();
    }
    T *operator=(T *desired) volatile noexcept
    {
        this->store(desired);
        return desired;
    }
    T *operator=(T *desired) noexcept
    {
        this->store(desired);
        return desired;
    }
    atomic &operator=(const atomic &) = delete;
private:
    friend void atomic_init<>(volatile atomic *obj, typename atomic::value_type desired) noexcept;
    friend void atomic_init<>(atomic *obj, typename atomic::value_type desired) noexcept;
};

using atomic_bool           = atomic<bool>;
using atomic_char           = atomic<char>;
using atomic_schar          = atomic<signed char>;
using atomic_uchar          = atomic<unsigned char>;
using atomic_char16_t       = atomic<char16_t>;
using atomic_char32_t       = atomic<char32_t>;
using atomic_wchar_t        = atomic<wchar_t>;
using atomic_short          = atomic<short>;
using atomic_ushort         = atomic<unsigned short>;
using atomic_int            = atomic<int>;
using atomic_uint           = atomic<unsigned int>;
using atomic_long           = atomic<long>;
using atomic_ulong          = atomic<unsigned long>;
using atomic_llong          = atomic<long long>;
using atomic_ullong         = atomic<unsigned long long>;
using atomic_int8_t         = atomic<int8_t>;
using atomic_uint8_t        = atomic<uint8_t>;
using atomic_int16_t        = atomic<int16_t>;
using atomic_uint16_t       = atomic<uint16_t>;
using atomic_int32_t        = atomic<int32_t>;
using atomic_uint32_t       = atomic<uint32_t>;
using atomic_int64_t        = atomic<int64_t>;
using atomic_uint64_t       = atomic<uint64_t>;
using atomic_int_least8_t   = atomic<int_least8_t>;
using atomic_uint_least8_t  = atomic<uint_least8_t>;
using atomic_int_least16_t  = atomic<int_least16_t>;
using atomic_uint_least16_t = atomic<uint_least16_t>;
using atomic_int_least32_t  = atomic<int_least32_t>;
using atomic_uint_least32_t = atomic<uint_least32_t>;
using atomic_int_least64_t  = atomic<int_least64_t>;
using atomic_uint_least64_t = atomic<uint_least64_t>;
using atomic_int_fast8_t    = atomic<int_fast8_t>;
using atomic_uint_fast8_t   = atomic<uint_fast8_t>;
using atomic_int_fast16_t   = atomic<int_fast16_t>;
using atomic_uint_fast16_t  = atomic<uint_fast16_t>;
using atomic_int_fast32_t   = atomic<int_fast32_t>;
using atomic_uint_fast32_t  = atomic<uint_fast32_t>;
using atomic_int_fast64_t   = atomic<int_fast64_t>;
using atomic_uint_fast64_t  = atomic<uint_fast64_t>;
using atomic_intptr_t       = atomic<intptr_t>;
using atomic_uintptr_t      = atomic<uintptr_t>;
using atomic_size_t         = atomic<size_t>;
using atomic_ptrdiff_t      = atomic<ptrdiff_t>;
using atomic_intmax_t       = atomic<intmax_t>;
using atomic_uintmax_t      = atomic<uintmax_t>;

template<typename T>
void atomic_init(atomic<T> *obj, typename atomic<T>::value_type desired) noexcept
{
    obj->init(desired);
}

template<typename T>
void atomic_init(volatile atomic<T> *obj, typename atomic<T>::value_type desired) noexcept
{
    obj->init(desired);
}

template<typename T>
bool atomic_is_lock_free(const atomic<T> *obj) noexcept
{
    return obj->is_lock_free();
}

template<typename T>
bool atomic_is_lock_free(const volatile atomic<T> *obj) noexcept
{
    return obj->is_lock_free();
}

template<typename T>
void atomic_store(atomic<T> *obj, typename atomic<T>::value_type desired) noexcept
{
    obj->store(desired);
}

template<typename T>
void atomic_store(volatile atomic<T> *obj, typename atomic<T>::value_type desired) noexcept
{
    obj->store(desired);
}

template<typename T>
void atomic_store_explicit(atomic<T> *obj, typename atomic<T>::value_type desired, memory_order order) noexcept
{
    obj->store(desired, order);
}

template<typename T>
void atomic_store_explicit(volatile atomic<T> *obj, typename atomic<T>::value_type desired, memory_order order) noexcept
{
    obj->store(desired, order);
}

template<typename T>
T atomic_load(const atomic<T> *obj) noexcept
{
    return obj->load();
}

template<typename T>
T atomic_load(const volatile atomic<T> *obj) noexcept
{
    return obj->load();
}

template<typename T>
T atomic_load_explicit(const atomic<T> *obj, memory_order order) noexcept
{
    return obj->load(order);
}

template<typename T>
T atomic_load_explicit(const volatile atomic<T> *obj, memory_order order) noexcept
{
    return obj->load(order);
}

template<typename T>
T atomic_exchange(atomic<T> *obj, typename atomic<T>::value_type desired) noexcept
{
    return obj->exchange(desired);
}

template<typename T>
T atomic_exchange(volatile atomic<T> *obj, typename atomic<T>::value_type desired) noexcept
{
    return obj->exchange(desired);
}

template<typename T>
T atomic_exchange_explicit(atomic<T> *obj, typename atomic<T>::value_type desired, memory_order order) noexcept
{
    return obj->exchange(desired, order);
}

template<typename T>
T atomic_exchange_explicit(volatile atomic<T> *obj, typename atomic<T>::value_type desired, memory_order order) noexcept
{
    return obj->exchange(desired, order);
}

template<typename T>
bool atomic_compare_exchange_weak(atomic<T> *obj,
                                  typename atomic<T>::value_type *currentExpected,
                                  typename atomic<T>::value_type desired) noexcept
{
    return obj->compare_exchange_weak(obj, *currentExpected, desired);
}

template<typename T>
bool atomic_compare_exchange_weak(volatile atomic<T> *obj,
                                  typename atomic<T>::value_type *currentExpected,
                                  typename atomic<T>::value_type desired) noexcept
{
    return obj->compare_exchange_weak(obj, *currentExpected, desired);
}

template<typename T>
bool atomic_compare_exchange_strong(atomic<T> *obj,
                                    typename atomic<T>::value_type *currentExpected,
                                    typename atomic<T>::value_type desired) noexcept
{
    return obj->compare_exchange_strong(obj, *currentExpected, desired);
}

template<typename T>
bool atomic_compare_exchange_strong(volatile atomic<T> *obj,
                                    typename atomic<T>::value_type *currentExpected,
                                    typename atomic<T>::value_type desired) noexcept
{
    return obj->compare_exchange_strong(obj, *currentExpected, desired);
}

template<typename T>
bool atomic_compare_exchange_weak_explicit(atomic<T> *obj,
                                           typename atomic<T>::value_type *currentExpected,
                                           typename atomic<T>::value_type desired,
                                           memory_order success,
                                           memory_order failure) noexcept
{
    return obj->compare_exchange_weak(obj, *currentExpected, desired, success, failure);
}

template<typename T>
bool atomic_compare_exchange_weak_explicit(volatile atomic<T> *obj,
                                           typename atomic<T>::value_type *currentExpected,
                                           typename atomic<T>::value_type desired,
                                           memory_order success,
                                           memory_order failure) noexcept
{
    return obj->compare_exchange_weak(obj, *currentExpected, desired, success, failure);
}

template<typename T>
bool atomic_compare_exchange_strong_explicit(atomic<T> *obj,
                                             typename atomic<T>::value_type *currentExpected,
                                             typename atomic<T>::value_type desired,
                                             memory_order success,
                                             memory_order failure) noexcept
{
    return obj->compare_exchange_strong(obj, *currentExpected, desired, success, failure);
}

template<typename T>
bool atomic_compare_exchange_strong_explicit(volatile atomic<T> *obj,
                                             typename atomic<T>::value_type *currentExpected,
                                             typename atomic<T>::value_type desired,
                                             memory_order success,
                                             memory_order failure) noexcept
{
    return obj->compare_exchange_strong(obj, *currentExpected, desired, success, failure);
}

template<typename T>
T atomic_fetch_add(atomic<T> *obj, typename atomic<T>::difference_type arg) noexcept
{
    return obj->fetch_add(arg);
}

template<typename T>
T atomic_fetch_add(volatile atomic<T> *obj, typename atomic<T>::difference_type arg) noexcept
{
    return obj->fetch_add(arg);
}

template<typename T>
T atomic_fetch_add_explicit(atomic<T> *obj, typename atomic<T>::difference_type arg, memory_order order) noexcept
{
    return obj->fetch_add(arg, order);
}

template<typename T>
T atomic_fetch_add_explicit(volatile atomic<T> *obj, typename atomic<T>::difference_type arg, memory_order order) noexcept
{
    return obj->fetch_add(arg, order);
}

template<typename T>
T atomic_fetch_sub(atomic<T> *obj, typename atomic<T>::difference_type arg) noexcept
{
    return obj->fetch_sub(arg);
}

template<typename T>
T atomic_fetch_sub(volatile atomic<T> *obj, typename atomic<T>::difference_type arg) noexcept
{
    return obj->fetch_sub(arg);
}

template<typename T>
T atomic_fetch_sub_explicit(atomic<T> *obj, typename atomic<T>::difference_type arg, memory_order order) noexcept
{
    return obj->fetch_sub(arg, order);
}

template<typename T>
T atomic_fetch_sub_explicit(volatile atomic<T> *obj, typename atomic<T>::difference_type arg, memory_order order) noexcept
{
    return obj->fetch_sub(arg, order);
}

template<typename T>
T atomic_fetch_and(atomic<T> *obj, typename atomic<T>::value_type arg) noexcept
{
    return obj->fetch_and(arg);
}

template<typename T>
T atomic_fetch_and(volatile atomic<T> *obj, typename atomic<T>::value_type arg) noexcept
{
    return obj->fetch_and(arg);
}

template<typename T>
T atomic_fetch_and_explicit(atomic<T> *obj, typename atomic<T>::value_type arg, memory_order order) noexcept
{
    return obj->fetch_and(arg, order);
}

template<typename T>
T atomic_fetch_and_explicit(volatile atomic<T> *obj, typename atomic<T>::value_type arg, memory_order order) noexcept
{
    return obj->fetch_and(arg, order);
}

template<typename T>
T atomic_fetch_or(atomic<T> *obj, typename atomic<T>::value_type arg) noexcept
{
    return obj->fetch_or(arg);
}

template<typename T>
T atomic_fetch_or(volatile atomic<T> *obj, typename atomic<T>::value_type arg) noexcept
{
    return obj->fetch_or(arg);
}

template<typename T>
T atomic_fetch_or_explicit(atomic<T> *obj, typename atomic<T>::value_type arg, memory_order order) noexcept
{
    return obj->fetch_or(arg, order);
}

template<typename T>
T atomic_fetch_or_explicit(volatile atomic<T> *obj, typename atomic<T>::value_type arg, memory_order order) noexcept
{
    return obj->fetch_or(arg, order);
}

template<typename T>
T atomic_fetch_xor(atomic<T> *obj, typename atomic<T>::value_type arg) noexcept
{
    return obj->fetch_xor(arg);
}

template<typename T>
T atomic_fetch_xor(volatile atomic<T> *obj, typename atomic<T>::value_type arg) noexcept
{
    return obj->fetch_xor(arg);
}

template<typename T>
T atomic_fetch_xor_explicit(atomic<T> *obj, typename atomic<T>::value_type arg, memory_order order) noexcept
{
    return obj->fetch_xor(arg, order);
}

template<typename T>
T atomic_fetch_xor_explicit(volatile atomic<T> *obj, typename atomic<T>::value_type arg, memory_order order) noexcept
{
    return obj->fetch_xor(arg, order);
}

struct atomic_flag {
    atomic_flag() noexcept = default;
    atomic_flag(const atomic_flag &) = delete;
    atomic_flag &operator=(const atomic_flag &) = delete;
    atomic_flag &operator=(const atomic_flag &) volatile = delete;
    bool test_and_set() volatile noexcept
    {
        return core_util_atomic_flag_test_and_set(&_flag);
    }
    bool test_and_set(memory_order order) volatile noexcept
    {
        return core_util_atomic_flag_test_and_set_explicit(&_flag, order);
    }
    void clear() volatile noexcept
    {
        core_util_atomic_flag_clear(&_flag);
    }
    void clear() noexcept
    {
        core_util_atomic_flag_clear(&_flag);
    }
    void clear(memory_order order) volatile noexcept
    {
        core_util_atomic_flag_clear_explicit(&_flag, order);
    }
    void clear(memory_order order) noexcept
    {
        core_util_atomic_flag_clear_explicit(&_flag, order);
    }
private:
    core_util_atomic_flag _flag;
};

MBED_FORCEINLINE bool atomic_flag_test_and_set(volatile atomic_flag *flag) noexcept
{
    return flag->test_and_set();
}

MBED_FORCEINLINE bool atomic_flag_test_and_set(atomic_flag *flag) noexcept
{
    return flag->test_and_set();
}

MBED_FORCEINLINE bool atomic_flag_test_and_set_explicit(volatile atomic_flag *flag, memory_order order) noexcept
{
    return flag->test_and_set(order);
}

MBED_FORCEINLINE bool atomic_flag_test_and_set_explicit(atomic_flag *flag, memory_order order) noexcept
{
    return flag->test_and_set(order);
}

MBED_FORCEINLINE void atomic_flag_clear(volatile atomic_flag *flag) noexcept
{
    flag->clear();
}

MBED_FORCEINLINE void atomic_flag_clear(atomic_flag *flag) noexcept
{
    flag->clear();
}

MBED_FORCEINLINE void atomic_flag_clear_explicit(volatile atomic_flag *flag, memory_order order) noexcept
{
    flag->clear(order);
}

MBED_FORCEINLINE void atomic_flag_clear_explicit(atomic_flag *flag, memory_order order) noexcept
{
    flag->clear(order);
}

#define MSTD_ATOMIC_FLAG_INIT { CORE_UTIL_ATOMIC_FLAG_INIT }

template<typename T>
T kill_dependency(T y) noexcept
{
    return y;
}

MBED_FORCEINLINE void atomic_signal_fence(memory_order order) noexcept
{
    if (order != memory_order_relaxed) {
        MBED_COMPILER_BARRIER();
    }
}

MBED_FORCEINLINE void atomic_thread_fence(memory_order order) noexcept
{
    if (order != memory_order_relaxed) {
        MBED_BARRIER();
    }
}
/**@}*/

/**@}*/

} // namespace mstd

#endif
