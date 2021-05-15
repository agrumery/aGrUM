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
 * @brief source file for virtual Base classes for oriented graphs listener
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/graphs/parts/listeners/diGraphListener.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/listeners/diGraphListener_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  DiGraphListener::DiGraphListener(const DiGraphListener& d) {
    GUM_CONS_CPY(DiGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for DiGraphListener")
  }

  DiGraphListener& DiGraphListener::operator=(const DiGraphListener& d) {
    GUM_OP_CPY(DiGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy operator for DiGraphListener")
  }

  DiGraphListener::DiGraphListener(const DiGraph* g) {
    if (!g) {
      GUM_ERROR(OperationNotAllowed, "A graph listener need a graph to listen to")
    }

    GUM_CONSTRUCTOR(DiGraphListener);
    graph_ = const_cast< DiGraph* >(g);

    GUM_CONNECT((*graph_), onNodeAdded, (*this), DiGraphListener::whenNodeAdded);
    GUM_CONNECT((*graph_),
                onNodeDeleted,
                (*this),
                DiGraphListener::whenNodeDeleted);
    GUM_CONNECT((*graph_), onArcAdded, (*this), DiGraphListener::whenArcAdded);
    GUM_CONNECT((*graph_), onArcDeleted, (*this), DiGraphListener::whenArcDeleted);
  }

  DiGraphListener::~DiGraphListener() {
    GUM_DESTRUCTOR(DiGraphListener);
    ;
  }

}   // namespace gum
