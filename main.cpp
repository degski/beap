
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <initializer_list>
#include <optional>
#include <random>
#include <sax/integer.hpp>
#include <sax/iostream.hpp>
#include <sax/splitmix.hpp>
#include <sax/uniform_int_distribution.hpp>
#include <span>
#include <tuple>
#include <type_traits>
#include <vector>

#include <plf/plf_nanotimer.h>

#include "beap.hpp"
#include "beap_view.hpp"

// clang-format off

std::vector<int> data = { 72,
                        68, 63,
                      44, 62, 55,
                    33, 22, 32, 51,
                  13, 18, 21, 19, 31,
                11, 12, 14, 17,  9, 13,
               3,  2, 10
};

/*

    72
    68 63
    44 62 55
    33 22 32 51
    13 18 21 19 31
    11 12 14 17 9 13
    3 2 10

    72 63 55 51 31 13
    68 62 32 19 9
    44 22 21 17
    33 18 14
    13 12 10
    11 2
    3

*/

// clang-format on

using size_type = int;

std::vector<size_type> test_data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };

// 0 1 3 6 9 10 15 21 28 36 45 55

using sp_t = tri::basic_span_type<int>;

inline std::array<sp_t, 64> const ref_span = {
    { sp_t{ 0, 0 },       sp_t{ 1, 2 },       sp_t{ 3, 5 },       sp_t{ 6, 9 },       sp_t{ 10, 14 },     sp_t{ 15, 20 },
      sp_t{ 21, 27 },     sp_t{ 28, 35 },     sp_t{ 36, 44 },     sp_t{ 45, 54 },     sp_t{ 55, 65 },     sp_t{ 66, 77 },
      sp_t{ 78, 90 },     sp_t{ 91, 104 },    sp_t{ 105, 119 },   sp_t{ 120, 135 },   sp_t{ 136, 152 },   sp_t{ 153, 170 },
      sp_t{ 171, 189 },   sp_t{ 190, 209 },   sp_t{ 210, 230 },   sp_t{ 231, 252 },   sp_t{ 253, 275 },   sp_t{ 276, 299 },
      sp_t{ 300, 324 },   sp_t{ 325, 350 },   sp_t{ 351, 377 },   sp_t{ 378, 405 },   sp_t{ 406, 434 },   sp_t{ 435, 464 },
      sp_t{ 465, 495 },   sp_t{ 496, 527 },   sp_t{ 528, 560 },   sp_t{ 561, 594 },   sp_t{ 595, 629 },   sp_t{ 630, 665 },
      sp_t{ 666, 702 },   sp_t{ 703, 740 },   sp_t{ 741, 779 },   sp_t{ 780, 819 },   sp_t{ 820, 860 },   sp_t{ 861, 902 },
      sp_t{ 903, 945 },   sp_t{ 946, 989 },   sp_t{ 990, 1034 },  sp_t{ 1035, 1080 }, sp_t{ 1081, 1127 }, sp_t{ 1128, 1175 },
      sp_t{ 1176, 1224 }, sp_t{ 1225, 1274 }, sp_t{ 1275, 1325 }, sp_t{ 1326, 1377 }, sp_t{ 1378, 1430 }, sp_t{ 1431, 1484 },
      sp_t{ 1485, 1539 }, sp_t{ 1540, 1595 }, sp_t{ 1596, 1652 }, sp_t{ 1653, 1710 }, sp_t{ 1711, 1769 }, sp_t{ 1770, 1829 },
      sp_t{ 1830, 1890 }, sp_t{ 1891, 1952 }, sp_t{ 1953, 2015 }, sp_t{ 2016, 2079 } }
};

int main ( ) {

    using beap_v = beap_view<int>;

    beap_v a ( data );

    //  for ( auto e : a )
    //       std::cout << e << ' ';
    //  std::cout << nl;

    std::cout << beap_v::is_beap ( data.begin ( ), data.end ( ) ) << nl;

    exit ( 0 );

    for ( int i = 0; i < 64; ++i ) // Test span implementation.
        assert ( ref_span[ i ].beg == tri::basic_span_type<int>::span ( i ).beg and
                 ref_span[ i ].end == tri::basic_span_type<int>::span ( i ).end );

    // Search for range of elements, found the printed ones.

    for ( int i = -10; i < 200; ++i )
        if ( int r = a.find ( i ); r != 27 )
            std::cout << i << ' ' << r << ' ';
        else
            std::cout << "E ";
    std::cout << nl;

    a.insert ( 54 );

    for ( auto e : a )
        std::cout << e << ' ';
    std::cout << nl;

    a.erase ( 54 );

    for ( auto e : a )
        std::cout << e << ' ';
    std::cout << nl;

    a.insert ( 69 );

    for ( auto e : a )
        std::cout << e << ' ';
    std::cout << nl;

    a.erase ( 69 );

    for ( auto e : a )
        std::cout << e << ' ';
    std::cout << nl;

    exit ( 0 );

    /*
    for ( auto r : data )
        std::cout << r << ' ';
    std::cout << nl;

    a.insert ( 67 );

    for ( auto r : data )
        std::cout << r << ' ';
    std::cout << nl;

    // a.insert ( 9 );
    // a.insert ( 91 );
    // a.insert ( 89 );
    // a.insert ( 19 );
    */
    return EXIT_SUCCESS;
}

/*
  search: idx: 21 s 21 e 27
  search: idx: 21 arr[idx]: 3
  Moving down v s 21 e 27
  search: idx: 15 s 15 e 20
  search: idx: 15 arr[idx]: 11
  Moving down v s 15 e 20
  search: idx: 10 s 10 e 14
  search: idx: 10 arr[idx]: 13
  Moving down v s 10 e 14
  search: idx: 6 s 6 e 9
  search: idx: 6 arr[idx]: 33
  Moving down v s 6 e 9
  search: idx: 3 s 3 e 5
  search: idx: 3 arr[idx]: 44
  Moving down v s 3 e 5
  search: idx: 1 s 1 e 2
  search: idx: 1 arr[idx]: 68
  Moving right -> s 1 e 2
  search: idx: 4 s 3 e 5
  search: idx: 4 arr[idx]: 62
  Moving right -> s 3 e 5
  search: idx: 8 s 6 e 9
  search: idx: 8 arr[idx]: 32
  Moving down v s 6 e 9
  search: idx: 5 s 3 e 5
  search: idx: 5 arr[idx]: 55
  Moving down v s 3 e 5
  Can't move down s 3 e 5
  None
*/

#if 0

int main ( ) {

    constexpr int size = 1'024;

    sax::splitmix64 rng{ [] ( ) {
        std::random_device rdev;
        return ( static_cast<std::size_t> ( rdev ( ) ) << 32 ) | static_cast<std::size_t> ( rdev ( ) );
    }( ) };
    sax::uniform_int_distribution<int> dis_lev{ 3, size * size - 1 };
    sax::uniform_int_distribution<std::size_t> dis_idx{ 0, size - 1 };

    plf::nanotimer t;

    t.start ( );

    for ( int i = 0; i < size * size; ++i ) {
    }

    uint64_t time = static_cast<uint64_t> ( t.get_elapsed_ms ( ) );

    std::cout << time << " ms " << nl;

    return EXIT_SUCCESS;
}

#endif
