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
 * @brief source file for virtual Base classes for non-oriented graphs listener
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/graphs/parts/listeners/undiGraphListener.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/listeners/undiGraphListener_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  UndiGraphListener::UndiGraphListener(const UndiGraphListener& d) {
    GUM_CONS_CPY(UndiGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for UndiGraphListener")
  }

  UndiGraphListener& UndiGraphListener::operator=(const UndiGraphListener& d) {
    GUM_OP_CPY(UndiGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy operator for UndiGraphListener")
  }

  UndiGraphListener::UndiGraphListener(UndiGraph* g) {
    if (!g) {
      GUM_ERROR(OperationNotAllowed, "A graph listener need a graph to listen to")
    }

    GUM_CONSTRUCTOR(UndiGraphListener);
    graph_ = g;

    GUM_CONNECT((*graph_), onNodeAdded, (*this), UndiGraphListener::whenNodeAdded);
    GUM_CONNECT((*graph_),
                onNodeDeleted,
                (*this),
                UndiGraphListener::whenNodeDeleted);
    GUM_CONNECT((*graph_), onEdgeAdded, (*this), UndiGraphListener::whenEdgeAdded);
    GUM_CONNECT((*graph_),
                onEdgeDeleted,
                (*this),
                UndiGraphListener::whenEdgeDeleted);
  }

  UndiGraphListener::~UndiGraphListener() { GUM_DESTRUCTOR(UndiGraphListener); }

}   // namespace gum
