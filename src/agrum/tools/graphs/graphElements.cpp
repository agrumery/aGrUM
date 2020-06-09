
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Outlined implementation of directed and undirected edges
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <iostream>
#include <sstream>

#include <agrum/agrum.h>

#include <agrum/tools/graphs/graphElements.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/graphElements_inl.h>
#endif /* GUM_NO_INLINE */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  ////////////////////////////////////////////////////////////////
  // we need to provide hash functions for some Edge and Arc

  /// A \c << operator for edges

  std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
    return (stream << edge.first() << "--" << edge.second());
  }

  /// A \c << operator for arcs

  std::ostream& operator<<(std::ostream& stream, const Arc& arc) {
    return (stream << arc.first() << "->" << arc.second());
  }

} /* namespace gum */

#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
template class gum::HashFunc< gum::NodeSet >;
#  endif

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
