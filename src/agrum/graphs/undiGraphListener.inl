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
 * @brief Inline implementation fo base classes for oriented graphs listener
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/graphs/undiGraphListener.h>

namespace gum {

UndiGraphListener::UndiGraphListener(const UndiGraphListener& d) {
    GUM_CONS_CPY(UndiGraphListener);
    GUM_ERROR(OperationNotAllowed,"No copy constructor for UndiGraphListener");
}

UndiGraphListener& UndiGraphListener::operator=(const UndiGraphListener& d) {
    GUM_OP_CPY(UndiGraphListener);
    GUM_ERROR(OperationNotAllowed,"No copy operator for UndiGraphListener");
}

UndiGraphListener::UndiGraphListener(UndiGraph* g) {
    GUM_CONSTRUCTOR(UndiGraphListener);
    if (! g) GUM_ERROR(OperationNotAllowed,"A graph listener need a graph to listen to");
    _digraph=g;
    GUM_CONNECT( (*_digraph),onNodeAdded,(*this),UndiGraphListener::whenNodeAdded );
    GUM_CONNECT( (*_digraph),onNodeDeleted,(*this),UndiGraphListener::whenNodeDeleted );
    GUM_CONNECT( (*_digraph),onEdgeAdded ,(*this),UndiGraphListener::whenEdgeAdded );
    GUM_CONNECT( (*_digraph),onEdgeDeleted,(*this),UndiGraphListener::whenEdgeDeleted );
}
UndiGraphListener::~UndiGraphListener() {
    GUM_DESTRUCTOR(UndiGraphListener);
}

} // namespace gum
