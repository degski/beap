
// MIT License
//
// Copyright (c) 2020 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <limits>
#include <type_traits>

#include "impl/preprocessor/iteration/local.hpp"

namespace tri {

template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr T prev_power_2 ( T value_ ) noexcept {
    value_ |= ( value_ >> 1 );
    value_ |= ( value_ >> 2 );
    if constexpr ( sizeof ( T ) > 1 ) {
        value_ |= ( value_ >> 4 );
    }
    if constexpr ( sizeof ( T ) > 2 ) {
        value_ |= ( value_ >> 8 );
    }
    if constexpr ( sizeof ( T ) > 4 ) {
        value_ |= ( value_ >> 16 );
    }
    return value_;
}

template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr T next_power_2 ( T value_ ) noexcept {
    return prev_power_2 ( value_ ) + 1;
}

template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr bool is_power_2 ( T const n_ ) noexcept {
    return n_ and not( n_ & ( n_ - 1 ) );
}

template<typename T, typename = std::enable_if_t<std::conjunction_v<std::is_integral<T>, std::is_unsigned<T>>>>
constexpr std::uint32_t pop_count ( T const x_ ) noexcept {
    if constexpr ( std::is_same<T, std::uint64_t>::value ) {
        return ( std::uint32_t ) __popcnt64 ( x_ );
    }
    else {
        return ( std::uint32_t ) __popcnt ( ( std::uint32_t ) x_ );
    }
}

// Integer square root.
template<typename SizeType>
[[nodiscard]] constexpr SizeType isqrt ( SizeType const val_ ) noexcept {
    assert ( val_ > 0 );
    return static_cast<SizeType> ( std::sqrt ( static_cast<double> ( val_ ) ) );
}

template<typename SizeType>
[[nodiscard]] constexpr SizeType nth_triangular_impl ( SizeType r_ ) noexcept {
    r_ += 1;
    return r_ * ( r_ - 1 ) / 2;
}

template<typename SizeType>
[[nodiscard]] constexpr SizeType nth_triangular_root_impl ( SizeType n_ ) noexcept {
    return ( isqrt ( 8 * n_ ) + 1 ) / 2;
}

// clang-format off

#define BOOST_PP_LOCAL_MACRO( n )                                                                                                  \
    case n: {                                                                                                                      \
        return nth_triangular_impl ( n );                                                                                          \
    }
#define BOOST_PP_LOCAL_LIMITS ( 1, ( 64 - 1 ) )

template<typename SizeType>
[[nodiscard]] SizeType nth_triangular ( SizeType r_ ) noexcept {
    switch ( r_ ) {
        case 0: return 0;
        #include BOOST_PP_LOCAL_ITERATE( )
        default:;
    }
    return nth_triangular_impl ( r_ );
}

#undef BOOST_PP_LOCAL_MACRO

#define BOOST_PP_LOCAL_MACRO( n )                                                                                                  \
    case n: {                                                                                                                      \
        return nth_triangular_root_impl ( n );                                                                                     \
    }
#define BOOST_PP_LOCAL_LIMITS ( 1, ( 64 * 64 - 1 ) )

template<typename SizeType>
[[nodiscard]] SizeType nth_triangular_root ( SizeType r_ ) noexcept {
    switch ( r_ ) {
        case 0: return 0;
        #include BOOST_PP_LOCAL_ITERATE( )
        default:;
    }
    return nth_triangular_root_impl ( r_ );
}

#undef BOOST_PP_LOCAL_MACRO

// clang-format on

template<typename SizeType>
[[nodiscard]] constexpr bool is_triangular ( SizeType i_ ) noexcept {
    return nth_triangular ( nth_triangular_root ( i_ ) ) == i_;
}

template<typename SizeType>
[[nodiscard]] constexpr SizeType nth_triangular_floor ( SizeType n_ ) noexcept {
    return nth_triangular ( nth_triangular_root ( n_ ) + 0 );
}
template<typename SizeType>
[[nodiscard]] constexpr SizeType nth_triangular_ceil ( SizeType n_ ) noexcept {
    return nth_triangular ( nth_triangular_root ( n_ ) + 1 );
}

template<typename SizeType>
struct span_type {
    SizeType begin, end;
};

template<typename SizeType>
[[nodiscard]] constexpr span_type<SizeType> nth_triangular_floor_ceil ( SizeType n_ ) noexcept {
    SizeType ceil = nth_triangular_root ( n_ ), floor = nth_triangular ( ceil );
    ceil += floor;
    return span_type<SizeType>{ std::move ( floor ), std::move ( ceil ) };
}

template<typename SizeType>
[[nodiscard]] constexpr span_type<SizeType> nth_triangular_next_floor_ceil ( SizeType n_ ) noexcept {
    return nth_triangular_floor_ceil ( n_ + 1 );
}
template<typename SizeType>
[[nodiscard]] constexpr span_type<SizeType> nth_triangular_prev_floor_ceil ( SizeType n_ ) noexcept {
    return nth_triangular_floor_ceil ( n_ - 1 );
}

template<typename T>
struct basic_span_type {

    using size_type = T;

    size_type beg, end;

    basic_span_type ( std::tuple<size_type, size_type> const & t_ ) noexcept :
        basic_span_type ( std::get<0> ( t_ ), std::get<1> ( t_ ) ) {}
    basic_span_type ( size_type b_, size_type e_ ) noexcept : beg ( b_ ), end ( e_ ) {}
    basic_span_type ( size_type lev_ ) noexcept : beg ( tri::nth_triangular ( lev_ ) ), end ( lev_ + beg ) {}

    [[maybe_unused]] basic_span_type & operator++ ( ) noexcept {
        size_type tmp = end + 1;
        end           = 2 * tmp - beg;
        beg           = tmp;
        return *this;
    }
    [[maybe_unused]] basic_span_type & operator-- ( ) noexcept {
        size_type tmp = 2 * beg - end;
        end           = beg - 1;
        beg           = tmp;
        return *this;
    }

    [[nodiscard]] BEAP_PURE basic_span_type next ( ) const noexcept {
        size_type tmp = end + 1;
        return { tmp, 2 * tmp - beg };
    }
    [[nodiscard]] BEAP_PURE basic_span_type prev ( ) const noexcept { return { 2 * beg - end, beg - 1 }; }

    [[nodiscard]] static constexpr BEAP_PURE basic_span_type span ( size_type lev_ ) noexcept {
        size_type beg = tri::nth_triangular ( lev_ );
        lev_ += beg;
        return { std::move ( beg ), std::move ( lev_ ) };
    }
};

} // namespace tri
