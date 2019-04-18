
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Implementation of the gumArcGraphPart
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/graphs/parts/arcGraphPart.h>

namespace gum {

  template < typename VAL >
  INLINE ArcProperty< VAL > ArcGraphPart::arcsProperty(VAL (*f)(const Arc&),
                                                       Size size) const {
    return __arcs.hashMap(f, size);
  }

  template < typename VAL >
  INLINE ArcProperty< VAL > ArcGraphPart::arcsProperty(const VAL& val,
                                                       Size       size) const {
    return __arcs.hashMap(val, size);
  }

  /// a method to create a list of Assent from a node list
  template < typename VAL >
  INLINE List< VAL > ArcGraphPart::listMapArcs(VAL (*f)(const Arc&)) const {
    return __arcs.listMap(f);
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
