/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/** @file
 * @brief Implementation of the EdgeGraphPart
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/tools/graphs/parts/edgeGraphPart.h>

namespace gum {

  template < typename VAL >
  INLINE EdgeProperty< VAL > EdgeGraphPart::edgesProperty(VAL (*f)(const Edge&),
                                                          Size size) const {
    return edges__.hashMap(f, size);
  }

  template < typename VAL >
  INLINE EdgeProperty< VAL > EdgeGraphPart::edgesProperty(const VAL& val,
                                                          Size       size) const {
    return edges__.hashMap(val, size);
  }

  template < typename VAL >
  INLINE List< VAL > EdgeGraphPart::listMapEdges(VAL (*f)(const Edge&)) const {
    return edges__.listMap(f);
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
