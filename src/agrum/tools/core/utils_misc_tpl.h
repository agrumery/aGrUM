/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Template implementation for utilities for aGrUM.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to help IDE parser
#include <agrum/tools/core/utils_misc.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

namespace std {
  template < typename T >
  ostream& operator<<(ostream& stream, const vector< T >& val) {
    bool deja = false;
    stream << "[";

    for (const auto& v: val) {
      if (deja) stream << " , ";
      else deja = true;
      stream << v;
    }

    stream << "]";

    return stream;
  }

  template < typename T1, typename T2 >
  ostream& operator<<(ostream& stream, const pair< T1, T2 >& val) {
    stream << "(" << val.first << "," << val.second << ")";
    return stream;
  }

  template < size_t N >
  struct _auxiliary_print_tuple_ {
    template < typename... T >
    static typename std::enable_if< (N < sizeof...(T)) >::type print(std::ostream&             os,
                                                                     const std::tuple< T... >& t) {
      char quote
         = (std::is_convertible< decltype(std::get< N >(t)), std::string >::value) ? '"' : 0;
      os << ", " << quote << std::get< N >(t) << quote;
      _auxiliary_print_tuple_< N + 1 >::print(os, t);
    }
    template < typename... T >
    static typename std::enable_if< !(N < sizeof...(T)) >::type print(std::ostream&,
                                                                      const std::tuple< T... >&) {}
  };

  template < typename T0, typename... T >
  std::ostream& operator<<(std::ostream& os, const std::tuple< T0, T... >& t) {
    char quote = (std::is_convertible< T0, std::string >::value) ? '"' : 0;
    os << '(' << quote << std::get< 0 >(t) << quote;
    _auxiliary_print_tuple_< 1 >::print(os, t);
    return os << ')';
  }

  template < class T >
  bool hasUniqueElts(std::vector< T > const& x) {
    if (x.size() <= 1) return true;
    if (x.size() == 2) return x[0] != x[1];

    auto refless = [](T const* l, T const* r) { return *l < *r; };
    auto refeq   = [](T const* l, T const* r) { return *l == *r; };

    std::vector< T const* > vp;
    vp.reserve(x.size());
    for (size_t i = 0; i < x.size(); ++i)
      vp.push_back(&x[i]);
    sort(vp.begin(), vp.end(), refless);   // O(N log N)
    // if no adjacent pair (vp_n,vp_n+1) has *vp_n == *vp_n+1
    return std::adjacent_find(vp.begin(), vp.end(), refeq) == vp.end();
  }
} /* namespace std */
