
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief source file for virtual Base classes for mixed graphs listener
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6)
 */

#include <agrum/tools/graphs/parts/listeners/mixedGraphListener.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/listeners/mixedGraphListener_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  MixedGraphListener::MixedGraphListener(const MixedGraphListener& d) {
    GUM_CONS_CPY(MixedGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for MixedGraphListener");
  }

  MixedGraphListener& MixedGraphListener::operator=(const MixedGraphListener& d) {
    GUM_OP_CPY(MixedGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy operator for MixedGraphListener");
  }

  MixedGraphListener::MixedGraphListener(MixedGraph* g) {
    if (!g) {
      GUM_ERROR(OperationNotAllowed, "A graph listener need a graph to listen to");
    }

    GUM_CONSTRUCTOR(MixedGraphListener);
    _graph = g;

    GUM_CONNECT(
       (*_graph), onNodeAdded, (*this), MixedGraphListener::whenNodeAdded);
    GUM_CONNECT(
       (*_graph), onNodeDeleted, (*this), MixedGraphListener::whenNodeDeleted);
    GUM_CONNECT((*_graph), onArcAdded, (*this), MixedGraphListener::whenArcAdded);
    GUM_CONNECT(
       (*_graph), onArcDeleted, (*this), MixedGraphListener::whenArcDeleted);
    GUM_CONNECT(
       (*_graph), onEdgeAdded, (*this), MixedGraphListener::whenEdgeAdded);
    GUM_CONNECT(
       (*_graph), onEdgeDeleted, (*this), MixedGraphListener::whenEdgeDeleted);
  }

  MixedGraphListener::~MixedGraphListener() { GUM_DESTRUCTOR(MixedGraphListener); }

}   // namespace gum
