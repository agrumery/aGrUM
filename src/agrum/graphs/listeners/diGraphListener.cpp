/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief source file for virtual Base classes for oriented graphs listener
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/listeners/diGraphListener.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/listeners/diGraphListener.inl>
#endif // GUM_NOINLINE

namespace gum {

  DiGraphListener::DiGraphListener(const DiGraphListener &d) {
    GUM_CONS_CPY(DiGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for DiGraphListener");
  }

  DiGraphListener &DiGraphListener::operator=(const DiGraphListener &d) {
    GUM_OP_CPY(DiGraphListener);
    GUM_ERROR(OperationNotAllowed, "No copy operator for DiGraphListener");
  }

  DiGraphListener::DiGraphListener(const DiGraph *g) {
    if (!g) {
      GUM_ERROR(OperationNotAllowed, "A graph listener need a graph to listen to");
    }

    GUM_CONSTRUCTOR(DiGraphListener);
    _graph = const_cast<DiGraph *>(g);

    GUM_CONNECT((*_graph), onNodeAdded, (*this), DiGraphListener::whenNodeAdded);
    GUM_CONNECT((*_graph), onNodeDeleted, (*this), DiGraphListener::whenNodeDeleted);
    GUM_CONNECT((*_graph), onArcAdded, (*this), DiGraphListener::whenArcAdded);
    GUM_CONNECT((*_graph), onArcDeleted, (*this), DiGraphListener::whenArcDeleted);
  }

  DiGraphListener::~DiGraphListener() { GUM_DESTRUCTOR(DiGraphListener); }

} // namespace gum
