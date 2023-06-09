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
#ifndef MSTD_SPAN_
#define MSTD_SPAN_

/* <mstd_span>
 *
 * - Includes toolchain's <span> if available
 * - Otherwise provides an implementation of a C++20 equivalent std::span
 *   - deduction guides available from C++17 and on
 * - Provides nonstandard mstd::make_span functions to allow deduction pre C++17
 *   - mstd::make_span functions stay available after C++17 for backwards compatibility
 */

#if __cplusplus >= 201703L && __has_include(<span>)
#include <version>
#endif

#if __cpp_lib_span >= 202002L
#include <span>

namespace mstd {
    using std::span;
    using std::dynamic_extent;
}

#else //__cpp_lib_span >= 202002L

#include <array>
#include "mstd_iterator.h"
#include "mstd_type_traits.h"
#include "mbed_assert.h"

namespace mstd {

constexpr size_t dynamic_extent = -1;

template <typename, size_t = dynamic_extent>
class span;

namespace detail {

template<typename ElementType, size_t Extent>
struct storage {
    constexpr storage() noexcept = default;

    constexpr storage(ElementType *ptr, size_t) noexcept :
        _data(ptr)
    {}

    ElementType *_data = nullptr;
    static constexpr size_t _size = Extent;
};
template<typename ElementType, size_t Extent>
constexpr size_t storage<ElementType, Extent>::_size;

template<typename ElementType>
struct storage<ElementType, dynamic_extent> {
    constexpr storage() noexcept = default;

    constexpr storage(ElementType *ptr, size_t size) noexcept :
        _data(ptr), _size(size)
    {}

    ElementType *_data = nullptr;
    size_t _size = 0;
};

template<typename>
struct is_span: std::false_type {};

template<typename T, size_t E>
struct is_span<span<T, E>>: std::true_type {};

template<typename>
struct is_std_array: std::false_type {};

template<typename T, size_t N>
struct is_std_array<std::array<T, N>>: std::true_type {};

template<typename, typename = void>
struct has_size : std::false_type {};

template<typename T>
struct has_size<T, void_t<decltype(mstd::size(std::declval<T>()))>>:
    std::true_type {};

template<typename, typename = void>
struct has_data : std::false_type {};

template<typename T>
struct has_data<T, void_t<decltype(mstd::data(std::declval<T>()))>>:
    std::true_type {};

template<typename T, typename U = mstd::remove_cvref_t<T>>
struct is_container{
    static constexpr bool value =
        !is_span<U>::value && !is_std_array<U>::value &&
            !std::is_array<U>::value && has_size<T>::value &&
                has_data<T>::value;
};

template <class T>
using iterator_t = decltype(mstd::begin(std::declval<T&>()));

template <class R>
using range_reference_t = mstd::iter_reference_t<iterator_t<R>>;

template <typename, typename, typename = void>
struct is_compatible : std::false_type {};

template <typename T, typename E>
struct is_compatible<T, E,
    typename mstd::enable_if_t<!mstd::is_same<
            typename mstd::remove_cv_t<
                decltype(mstd::data(std::declval<T>()))
            >, void>::value>>:
mstd::is_convertible<remove_pointer_t<
    decltype(mstd::data(std::declval<T>()))> (*)[], E (*)[]>{};

} // namespace detail

template<typename ElementType, size_t Extent>
class span {
public:
    using element_type = ElementType;
    using value_type = typename mstd::remove_cv_t<element_type>;
    using index_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = element_type *;
    using const_pointer = const element_type *;
    using reference = element_type &;
    using const_reference = const element_type &;
    using iterator = pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;

    static constexpr index_type extent = Extent;

    // Constructors, copy and assignment
    template<size_t E = Extent,
        typename mstd::enable_if_t<E == dynamic_extent, int> = 0>
    constexpr span() noexcept
    {}

    template<class It, size_t E = Extent,
        typename mstd::enable_if_t<mstd::is_convertible<
        remove_reference_t<mstd::iter_reference_t<It>>(*)[],
        ElementType(*)[]>::value, int> = 0>
    constexpr span(It ptr, index_type count) :
        _storage(ptr, count)
    {
        MBED_ASSERT(extent == dynamic_extent || extent == count);
    }

    template<class It, typename mstd::enable_if_t<mstd::is_convertible<
        remove_reference_t<mstd::iter_reference_t<It>>(*)[],
        ElementType(*)[]>::value, int> = 0>
    constexpr span(It first, It last) :
        _storage(first, last - first)
    {
        MBED_ASSERT(first <= last);
        MBED_ASSERT(extent == dynamic_extent || extent == last - first);
        MBED_ASSERT(extent == 0 || nullptr != first);
    }

    template<size_t N>
    constexpr span(type_identity_t<element_type>(&arr)[N],
            typename mstd::enable_if_t<(Extent == dynamic_extent || Extent == N) &&
            mstd::is_convertible<remove_pointer_t<decltype(mstd::data(arr))>(*)[],
            ElementType(*)[]>::value, int> = 0) noexcept:
        _storage(arr, N)
    {}

    template<class T, size_t N>
    constexpr span(std::array<T, N> &arr,
            typename mstd::enable_if_t<(Extent == dynamic_extent || Extent == N) &&
            mstd::is_convertible<remove_pointer_t<decltype(mstd::data(arr))>(*)[],
            ElementType(*)[]>::value, int> = 0) noexcept:
        _storage(arr.data(), N)
    {}

    template<class T, size_t N>
    constexpr span(const std::array<T, N> &arr,
            typename mstd::enable_if_t<(Extent == dynamic_extent || Extent == N) &&
            mstd::is_convertible<remove_pointer_t<decltype(mstd::data(arr))>(*)[],
            ElementType(*)[]>::value, int> = 0) noexcept:
        _storage(arr.data(), N)
    {}

    template<class R,
    typename mstd::enable_if_t<detail::is_container<R>::value &&
            detail::is_compatible<R&, ElementType>::value, int> = 0>
    constexpr span(R &&r) :
        _storage(mstd::data(r), mstd::size(r))
    {
        MBED_ASSERT(extent == dynamic_extent || extent == mstd::size(r));
    }

    constexpr span(const span &other) noexcept = default;

    template<class OtherElementType, size_t OtherExtent,
    typename mstd::enable_if_t<(Extent == dynamic_extent || OtherExtent == Extent) &&
        mstd::is_convertible<OtherElementType(*)[], ElementType(*)[]>::value, int> = 0>
    constexpr span(const span<OtherElementType, OtherExtent>& s) noexcept:
        _storage(s.data(), s.size())
    {}

    ~span() noexcept = default;

    constexpr span& operator=(const span &other) noexcept = default;

    // Subviews
    template<size_t Count>
    constexpr span<element_type, Count> first() const
    {
        static_assert(Count <= extent);
        MBED_ASSERT(Count <= size());
        return {data(), Count};
    }

    template<size_t Count>
    constexpr span<element_type, Count> last() const
    {
        static_assert(Count <= extent);
        MBED_ASSERT(Count <= size());
        return {data() + (size() - Count), Count};
    }

    template<size_t Offset, size_t Count = dynamic_extent>
    constexpr span<element_type, Count != dynamic_extent ? Count
        : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent)> subspan() const
    {
        static_assert(Offset <= extent && (Count == dynamic_extent || Count <= extent - Offset));
        // Only check against Offset == 0 to prevent a warning for subspan<0, N>
        MBED_ASSERT((Offset == 0 || Offset <= size())
                    && (Count == dynamic_extent || Count <= size() - Offset));
        return {data() + Offset, Count != dynamic_extent ? Count : size() - Offset};
    }

    constexpr span<element_type, dynamic_extent> first(index_type count) const
    {
        MBED_ASSERT(count <= size());
        return {data(), count};
    }

    constexpr span<element_type, dynamic_extent> last(index_type count) const
    {
        MBED_ASSERT(count <= size());
        return {data() + (size() - count), count};
    }

    constexpr span<element_type, dynamic_extent>
        subspan(index_type offset, index_type count = dynamic_extent) const
    {
        MBED_ASSERT(offset <= size() && (count == dynamic_extent || count <= size() - offset));
        return {data() + offset, count == dynamic_extent ? size() - offset : count };
    }

    // Observers
    constexpr index_type size() const noexcept
    {
        return _storage._size;
    }

    constexpr index_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    // Element access
    constexpr reference operator[](index_type idx) const
    {
        MBED_ASSERT(idx < size());
        return *(data() + idx);
    }

    constexpr reference front() const
    {
        MBED_ASSERT(!empty());
        return *data();
    }

    constexpr reference back() const
    {
        MBED_ASSERT(!empty());
        return *(data() + (size() - 1));
    }

    constexpr pointer data() const noexcept
    {
        return _storage._data;
    }

    // Iterators
    constexpr iterator begin() const noexcept
    {
        return data();
    }

    constexpr iterator end() const noexcept
    {
        return data() + size();
    }

    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

private:
    detail::storage<element_type, extent> _storage;
};

template<typename ElementType, size_t Extent>
constexpr span<ElementType, Extent>::index_type span<ElementType, Extent>::extent;

#if __cplusplus >= 201703L || __cpp_deduction_guides >= 201703L
// Deduction guides
template<class It, class EndOrSize>
span(It, EndOrSize) -> span<remove_reference_t<iter_reference_t<It>>>;

template<class T, size_t N>
span(T (&)[N]) -> span<T, N>;

template<class T, size_t N>
span(std::array<T, N>&) -> span<T, N>;

template<class T, size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template<class R>
span(R&&) -> span<remove_reference_t<detail::range_reference_t<R>>>;
#endif //__cplusplus >= 201703L || __cpp_deduction_guides >= 201703L

} // namespace mstd

#endif //__cpp_lib_span >= 202002L

namespace mstd {

/** Create a span class with type and size inferred from the argument
 *
 *  @param arr      Reference to a c-style array
 *  @return         Span with inferred type and size Extent
 */
template<class ElementType, size_t Extent>
constexpr span<ElementType, Extent> make_span(ElementType (&arr)[Extent])
{
    return arr;
}

template<class ElementType, size_t Extent>
constexpr span<const ElementType, Extent> make_span(const ElementType (&arr)[Extent])
{
    return arr;
}

/** Create a span class with type and size inferred from the argument
 *
 *  @param arr      Reference to an std::array
 *  @return         Span with inferred type and size Extent
 */
template<class ElementType, size_t Extent>
constexpr span<ElementType, Extent> make_span(std::array<ElementType, Extent> &arr)
{
    return arr;
}

template<class ElementType, size_t Extent>
constexpr span<const ElementType, Extent> make_span(const std::array<ElementType, Extent> &arr)
{
    return arr;
}

/** Create a span class with type inferred from the argument
 *
 *  @param cont     Reference to a container
 *  @return         Span with inferred type and dynamic size
 */
template<class R>
constexpr span<typename R::value_type> make_span(R &cont)
{
    return cont;
}

template<class R>
constexpr span<const typename R::value_type> make_span(const R &cont)
{
    return cont;
}

} // namespace mstd

#endif // MSTD_SPAN_
