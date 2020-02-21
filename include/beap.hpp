
// MIT License
//
// Copyright (c) 2020 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, rhs_, modify, merge, publish, distribute, sublicense, and/or sell
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <compare>
#include <limits>
#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/hedley.hpp"

#define BEAP_PURE HEDLEY_PURE
#define BEAP_UNPREDICTABLE HEDLEY_UNPREDICTABLE
#define BEAP_LIKELY HEDLEY_LIKELY
#define BEAP_UNLIKELY HEDLEY_UNLIKELY

#include "detail/triangular.hpp"

#define ever                                                                                                                       \
    ;                                                                                                                              \
    ;

template<typename ValueType, typename SignedSizeType = int32_t, typename Compare = std::less<SignedSizeType>>
class beap {

    // Current beap_height of beap. Note that beap_height is defined as
    // distance between consecutive layers, so for single - element
    // beap beap_height is 0, and for empty, we initialize it to - 1.
    //
    // An example of the lay-out:
    //
    //    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27 28 29 30 ..
    // { 72, 68, 63, 44, 62, 55, 33, 22, 32, 51, 13, 18, 21, 19, 22, 11, 12, 14, 17,  9, 13,  3,  2, 10, 54 }
    //    _   _       _           _               _                   _                       _                          _

    public:
    using size_type = SignedSizeType;

    private:
    template<typename T, typename Comp>
    struct basic_value_type {

        T v;

        constexpr basic_value_type ( T value_ ) noexcept : v{ value_ } {}

        [[nodiscard]] constexpr size_type operator<=> ( basic_value_type const & r_ ) const noexcept {
            return static_cast<size_type> ( static_cast<int> ( not( Comp ( ) ( r_.v, v ) ) ) -
                                            static_cast<int> ( Comp ( ) ( v, r_.v ) ) );
        };

        template<typename Stream>
        [[maybe_unused]] friend Stream & operator<< ( Stream & out_, basic_value_type const & value_ ) noexcept {
            out_ << value_.v;
            return out_;
        }
    };

    using value_type     = basic_value_type<ValueType, Compare>;
    using container_type = std::vector<value_type>;

    public:
    using difference_type        = size_type;
    using reference              = typename container_type::reference;
    using const_reference        = typename container_type::const_reference;
    using pointer                = typename container_type::pointer;
    using const_pointer          = typename container_type::const_pointer;
    using iterator               = typename container_type::iterator;
    using const_iterator         = typename container_type::const_iterator;
    using reverse_iterator       = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;

    private:
    using span_type = tri::basic_span_type<size_type>;

    public:
    beap ( ) noexcept        = default;
    beap ( beap const & b_ ) = default;
    beap ( beap && b_ )      = default;

    template<typename ForwardIt>
    beap ( ForwardIt b_, ForwardIt e_ ) :
        data ( b_, e_ ),
        end_span ( span_type::span ( tri::nth_triangular_root ( static_cast<size_type> ( data.size ( ) ) ) - 1 ) ) {}

    [[maybe_unused]] beap & operator= ( beap const & b_ ) = default;
    [[maybe_unused]] beap & operator= ( beap && b_ ) = default;

    // Operations (private).

    private:
    [[nodiscard]] span_type search ( value_type const & v ) const noexcept {
        span_type s           = end_span;
        size_type beap_height = s.end - s.beg, i = s.beg + 1, h = beap_height, len = length ( );
        for ( ever ) {
            switch ( value_type const & d = data[ i ]; BEAP_UNPREDICTABLE ( v <=> d ) ) {
                case -1: {
                    if ( BEAP_UNLIKELY ( i == ( len - 1 ) ) ) {
                        --s, i -= h--;
                        continue;
                    }
                    if ( size_type i_ = i + h + 2; BEAP_UNLIKELY ( i_ < len ) ) {
                        ++s, i = i_, h += 1;
                        continue;
                    }
                    if ( BEAP_UNLIKELY ( i++ == ( s.end - 1 ) ) )
                        return { end_span.end, beap_height };
                    continue;
                }
                case +1: {
                    if ( BEAP_UNLIKELY ( i == s.end ) )
                        return { end_span.end, beap_height };
                    --s, i -= h--;
                    continue;
                }
                default: {
                    return { i, h };
                }
            }
        }
    }

    [[nodiscard]] size_type breadth_first_search ( value_type const & v_ ) noexcept {
        size_type siz = size ( );
        for ( size_type base_l = 0, base_i = tri::nth_triangular ( base_l ); BEAP_UNLIKELY ( base_i < siz );
              base_l += 1, base_i += base_l + 1 ) {
            for ( size_type lev = base_l + 1, l_i = base_i + 2, r_i = base_i + ( lev + 2 ) - 2;
                  BEAP_UNLIKELY ( l_i < siz and r_i < siz ); lev += 1, l_i += ( lev + 1 ), r_i += ( ( lev + 2 ) - 2 ) ) {
                if ( BEAP_UNLIKELY ( v_ == data[ l_i ] ) )
                    return l_i;
                if ( BEAP_UNLIKELY ( v_ == data[ r_i ] ) )
                    return r_i;
            }
        }
        return siz;
    }

    [[maybe_unused]] size_type bubble_up ( size_type i_, size_type h_ ) noexcept {
        span_type s = end_span;
        while ( BEAP_LIKELY ( h_ ) ) {
            span_type p = s.prev ( );
            size_type d = i_ - s.beg;
            size_type l = 0, r = 0;
            if ( BEAP_UNLIKELY ( i_ != s.beg ) )
                l = p.beg + d - 1;
            if ( BEAP_UNLIKELY ( i_ != s.end ) )
                r = p.beg + d;
            if ( BEAP_UNPREDICTABLE ( ( l ) and ( refof ( i_ ) > refof ( l ) ) and ( not r or ( refof ( l ) < refof ( r ) ) ) ) ) {
                std::swap ( refof ( i_ ), refof ( l ) );
                i_ = l;
            }
            else if ( BEAP_UNPREDICTABLE ( ( r ) and ( refof ( i_ ) > refof ( r ) ) ) ) {
                std::swap ( refof ( i_ ), refof ( r ) );
                i_ = r;
            }
            else {
                return i_;
            }
            s = p;
            h_ -= 1;
        }
        assert ( i_ == 0 );
        return i_;
    }

    [[nodiscard]] size_type bubble_down ( size_type i_, size_type h_ ) noexcept {
        span_type s         = end_span;
        size_type const h_1 = s.end - s.beg - 1, len = length ( );
        while ( BEAP_LIKELY ( h_ < h_1 ) ) {
            span_type c = s.next ( );
            size_type l = c.beg + i_ - s.beg, r = 0;
            if ( BEAP_LIKELY ( l < len ) ) {
                r = l + 1;
                if ( BEAP_UNLIKELY ( r >= len ) )
                    r = 0;
            }
            else {
                l = 0;
            }
            if ( BEAP_UNPREDICTABLE ( l and refof ( i_ ) < refof ( l ) and ( not r or refof ( l ) > refof ( r ) ) ) ) {
                std::swap ( refof ( i_ ), refof ( l ) );
                i_ = l;
            }
            else if ( BEAP_UNPREDICTABLE ( r and refof ( i_ ) < refof ( r ) ) ) {
                std::swap ( refof ( i_ ), refof ( r ) );
                i_ = r;
            }
            else {
                return i_;
            }
            s = c;
            h_ += 1;
        }
        return i_;
    }

    [[maybe_unused]] void erase_impl ( size_type i_, size_type h_ ) noexcept {
        size_type len = length ( );
        if ( BEAP_UNLIKELY ( len == end_span.beg ) ) {
            --end_span;
            shrink_to_fit ( ); // only when load is less than 50%.
        }
        assert ( i_ != ( len - 1 ) );
        refof ( i_ ) = pop_data ( );
        if ( size_type i = bubble_down ( i_, h_ ); BEAP_LIKELY ( i == i_ ) )
            bubble_up ( i_, h_ );
    }

    template<typename... Args>
    [[maybe_unused]] size_type emplace_impl ( size_type i_, Args... args_ ) noexcept {
        data.emplace_back ( std::forward<Args> ( args_ )... );
        return bubble_up ( i_, end_span.end - end_span.beg );
    }

    // Operations (public).

    public:
    [[maybe_unused]] size_type insert ( value_type const & v_ ) { return emplace ( value_type{ v_ } ); }
    template<typename ForwardIt>
    void insert ( ForwardIt b_, ForwardIt e_ ) noexcept {
        data.reserve ( static_cast<typename container_type::size_type> (
            tri::nth_triangular_ceil ( static_cast<size_type> ( data.size ( ) + std::distance ( b_, e_ ) ) ) ) );
        size_type c = size ( );
        while ( b_ != e_ )
            emplace_impl ( c++, value_type{ *b_ } );
    }

    // clang-format on

    template<typename... Args>
    [[maybe_unused]] size_type emplace ( Args... args_ ) {
        size_type i = length ( );
        if ( BEAP_UNLIKELY ( i == end_span.end ) ) {
            ++end_span;
            data.reserve ( end_span.end );
        }
        return emplace_impl ( i, std::forward<Args> ( args_ )... );
    }
    template<typename ForwardIt>
    [[maybe_unused]] void emplace ( ForwardIt b_, ForwardIt e_ ) noexcept {
        data.reserve ( static_cast<typename container_type::size_type> (
            tri::nth_triangular_ceil ( static_cast<size_type> ( data.size ( ) + std::distance ( b_, e_ ) ) ) ) );
        size_type c = size ( );
        while ( b_ != e_ )
            emplace_impl ( c++, std::move ( *b_ ) );
    }

    void erase ( value_type const & v_ ) noexcept {
        auto [ i, h ] = search ( v_ );
        if ( BEAP_UNLIKELY ( i == end_span.end ) )
            return;
        erase_impl ( i, h );
    }
    void erase_by_index ( size_type i_ ) noexcept {
        if ( BEAP_UNLIKELY ( i_ == end_span.end ) )
            return;
        erase_impl ( i_, tri::nth_triangular_root ( i_ ) );
    }

    [[nodiscard]] size_type find ( value_type const & v_ ) const noexcept { return search ( v_ ).beg; }
    [[nodiscard]] bool contains ( value_type const & v_ ) const noexcept { return find ( v_ ) != end_span.end; }

    // Sizes.

    [[nodiscard]] BEAP_PURE size_type size ( ) const noexcept { return static_cast<int> ( data.size ( ) ); }
    [[nodiscard]] BEAP_PURE size_type length ( ) const noexcept { return size ( ); }
    [[nodiscard]] BEAP_PURE size_type capacity ( ) const noexcept { return static_cast<int> ( data.capacity ( ) ); }

    void shrink_to_fit ( ) {
        if ( BEAP_UNLIKELY ( ( capacity ( ) >> 1 ) == size ( ) ) ) { // iff 100% over-allocated, force shrinking.
            container_type tmp;
            tmp.reserve ( end_span.end );
            data = std::move ( ( tmp = data ) );
        }
    }

    public:
    [[nodiscard]] BEAP_PURE iterator begin ( ) noexcept { return data.begin ( ); }
    [[nodiscard]] BEAP_PURE const_iterator cbegin ( ) const noexcept { return data.begin ( ); }

    [[nodiscard]] BEAP_PURE iterator end ( ) noexcept { return data.end ( ); }
    [[nodiscard]] BEAP_PURE const_iterator cend ( ) const noexcept { return data.end ( ); }

    [[nodiscard]] BEAP_PURE iterator rbegin ( ) noexcept { return data.rbegin ( ); }
    [[nodiscard]] BEAP_PURE const_iterator crbegin ( ) const noexcept { return data.rbegin ( ); }

    [[nodiscard]] BEAP_PURE iterator rend ( ) noexcept { return data.rend ( ); }
    [[nodiscard]] BEAP_PURE const_iterator crend ( ) const noexcept { return data.rend ( ); }

    // Beap.

    static inline void make_baep ( ) noexcept {
        end_span    = span_type::span ( 0 );
        size_type c = 1;
        iterator b = data.begin ( ) + 1, e = data.end ( );
        while ( b != e )
            emplace_impl ( c++, std::move ( *b ) );
    }

    [[nodiscard]] static inline ValueType pop_beap ( ) noexcept {
        after_exit_erase_top guard ( this );
        return data.front ( ).v;
    }

    [[maybe_unused]] size_type push_beap ( value_type const & v_ ) { return insert ( v_ ); }

    [[nodiscard]] BEAP_PURE reference top ( ) noexcept { return data.front ( ); }
    [[nodiscard]] BEAP_PURE const_reference top ( ) const noexcept { return data.front ( ); }

    [[nodiscard]] BEAP_PURE const_reference bottom ( ) const noexcept {
        for ( size_type min = end_span.beg, i = min + 1, data_end = size ( ); BEAP_LIKELY ( i < data_end ); ++i )
            if ( BEAP_UNPREDICTABLE ( data[ i ] < data[ min ] ) )
                min = i;
    }
    [[nodiscard]] BEAP_PURE reference bottom ( ) noexcept { return const_cast<reference> ( std::as_const ( this )->bottom ( ) ); }

    template<typename ForwardIt>
    [[nodiscard]] static ForwardIt is_beap_untill ( ForwardIt b_, ForwardIt e_ ) noexcept {

        auto const data = &*b_;

        //                          72,
        //                        68, 63,
        //                      44, 62, 55,
        //                    33, 22, 32, 51,
        //                  13, 18, 21, 19, 31,
        //                11, 12, 14, 17,  9, 13,
        //               3,  2, 10

        size_type size = static_cast<size_type> ( std::distance ( b_, e_ ) );
        // Searches for out-of-order element, top-down and breadth-first.
        for ( size_type base_l = 0, base_i = tri::nth_triangular ( base_l ); BEAP_UNLIKELY ( base_i < size );
              base_l += 1, base_i += base_l + 1 ) {
            for ( size_type lev = base_l + 1, l_p = base_i - lev + 1, l_i = l_p + ( lev ) + 1, r_p = base_i,
                            r_i = r_p + ( lev + 2 ) - 2;
                  BEAP_UNLIKELY ( l_i < size and r_i < size ); lev += 1, l_i += ( lev + 1 ), r_i += ( ( lev + 2 ) - 2 ) ) {
                if ( not BEAP_UNLIKELY ( Compare ( ) ( data[ l_i ], data[ l_p ] ) ) )
                    return b_ + l_i;
                if ( not BEAP_UNLIKELY ( Compare ( ) ( data[ r_i ], data[ r_p ] ) ) )
                    return b_ + r_i;
                l_p = l_i;
                r_p = r_i;
            }
        }
        return e_;
    }

    template<typename ForwardIt>
    [[nodiscard]] static bool is_beap ( ForwardIt b_, ForwardIt e_ ) noexcept {
        return is_beap_untill ( b_, e_ ) == e_;
    }

    [[nodiscard]] size_type is_beap_untill ( ) noexcept {
        // Searches for out-of-order element, top-down and breadth-first.
        size_type siz = size ( );
        for ( size_type base_l = 0, base_i = tri::nth_triangular ( base_l ); BEAP_UNLIKELY ( base_i < siz );
              base_l += 1, base_i += base_l + 1 ) {
            for ( size_type lev = base_l + 1, l_p = base_i - lev + 1, l_i = l_p + ( lev ) + 1, r_p = base_i,
                            r_i = r_p + ( lev + 2 ) - 2;
                  BEAP_UNLIKELY ( l_i < siz and r_i < siz ); lev += 1, l_i += ( lev + 1 ), r_i += ( ( lev + 2 ) - 2 ) ) {
                if ( not BEAP_UNLIKELY ( data[ l_i ] < data[ l_p ] ) )
                    return l_i;
                if ( not BEAP_UNLIKELY ( data[ r_i ] < data[ r_p ] ) )
                    return r_i;
                l_p = l_i;
                r_p = r_i;
            }
        }
        return siz;
    }

    // Miscelanious.

    void clear ( ) noexcept { data.clear ( ); }
    [[nodiscard]] constexpr size_type max_size ( ) const noexcept { return std::numeric_limits<size_type>::max ( ); }

    void swap ( beap & rhs_ ) noexcept { std::swap ( data, rhs_.data ); }

    [[nodiscard]] bool contains ( ValueType const & v_ ) const noexcept { return search ( v_ ).beg; }

    [[nodiscard]] bool empty ( ) const noexcept { return data.empty ( ); }

    // Output.

    template<typename Stream>
    [[maybe_unused]] friend Stream & operator<< ( Stream & out_, beap const & beap_ ) noexcept {
        std::for_each ( beap_.cbegin ( ), beap_.cend ( ), [ &out_ ] ( auto & e ) { out_ << e << sp; } );
        return out_;
    }

    // Miscelanious.

    private:
    [[nodiscard]] BEAP_PURE const_reference refof ( size_type i_ ) const noexcept { return ( data.data ( ) )[ i_ ]; }
    [[nodiscard]] BEAP_PURE reference refof ( size_type i_ ) noexcept { return ( data.data ( ) )[ i_ ]; }

    struct after_exit_erase_top {
        beap * c;
        after_exit_erase_top ( beap & c_ ) noexcept : c ( std::addressof ( c_ ) ) {}
        ~after_exit_erase_top ( ) noexcept { c->erase_impl ( 0, 1 ); };
    };

    struct after_exit_pop_back {
        container_type * c;
        after_exit_pop_back ( container_type & c_ ) noexcept : c ( std::addressof ( c_ ) ) {}
        ~after_exit_pop_back ( ) noexcept { c->pop_back ( ); }
    };

    [[nodiscard]] value_type pop_data ( ) noexcept {
        after_exit_pop_back guard ( data );
        return data.back ( );
    }

    // Members.

    container_type data;
    span_type end_span = { 0, 0 };
};

#undef PRIVATE
#undef PUBLIC

#undef BEAP_PURE
#undef BEAP_UNPREDICTABLE
#undef BEAP_LIKELY
#undef BEAP_UNLIKELY

#undef ever
