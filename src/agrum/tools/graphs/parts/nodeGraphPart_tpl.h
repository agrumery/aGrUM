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


/** @file
 * @brief Implementation of the NodeGraphPart
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum {

  template < typename VAL >
  INLINE NodeProperty< VAL > NodeGraphPart::nodesProperty(VAL (*f)(const NodeId&),
                                                          Size size) const {
    return asNodeSet().hashMap(f, size);
  }

  template < typename VAL >
  INLINE NodeProperty< VAL > NodeGraphPart::nodesProperty(const VAL& val, Size size) const {
    return asNodeSet().hashMap(val, size);
  }

  template < typename VAL >
  INLINE List< VAL > NodeGraphPart::listMapNodes(VAL (*f)(const NodeId&)) const {
    return asNodeSet().listMap(f);
  }

  template < typename T >
  void NodeGraphPart::populateNodesFromProperty(const NodeProperty< T >& h) {
    clear();

    for (const auto iter = h.begin(); iter != h.end(); ++iter) {
      NodeId new_one = iter.key();

      if (existsNode(new_one)) {
        GUM_ERROR(DuplicateElement, new_one << " already exists in the graph")
      }

      addNodeWithId(new_one);
    }
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
