
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
 * @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

INLINE
const EdgeSet& EliminationSequenceTriangulation::fillIns() {
    if (__fillIns == 0) {
        __computeEliminationTree();
    }
    return *__fillIns;
}

INLINE
const std::vector<NodeId>& EliminationSequenceTriangulation::eliminationOrder() {
    return __eliminationSequence;
}

INLINE
unsigned int EliminationSequenceTriangulation::eliminationOrder( const NodeId id )  {
    return __invertedElimination[id];
}

INLINE
const UndiGraph& EliminationSequenceTriangulation::triangulatedGraph() {
    if (__triangulatedGraph == 0) {
        __computeEliminationTree();
    }
    return *__triangulatedGraph;
}

INLINE
const CliqueGraph& EliminationSequenceTriangulation::eliminationTree() {
    if (__eliminationTree == 0) {
        __computeEliminationTree();
    }
    return *__eliminationTree;
}

INLINE
const CliqueGraph& EliminationSequenceTriangulation::junctionTree() {
    if (__junctionTree == 0) {
        __computeJunctionTree();
    }
    return *__junctionTree;
}

INLINE
NodeId EliminationSequenceTriangulation::createdClique( const NodeId id ) {
    if (__junctionTree == 0) {
        __computeJunctionTree();
    }
    return (*__createdCliqueTab)[id];
}

INLINE
void EliminationSequenceTriangulation::clear() {
    if (__fillIns != 0) {
        delete __fillIns;
    }
    if (__triangulatedGraph != 0) {
        delete __triangulatedGraph;
    }
    if (__eliminationTree != 0) {
        delete __eliminationTree;
    }
    if (__junctionTree != 0) {
        delete __junctionTree;
    }
    if (__createdCliqueTab != 0) {
        delete __createdCliqueTab;
    }
    __cliqueOrder.clear();
}

INLINE
void EliminationSequenceTriangulation::setGraph ( const UndiGraph& theGraph,
        const Property<unsigned int>::onNodes& modal ) {
    __graph = theGraph;
    clear();
}

INLINE
Triangulation* EliminationSequenceTriangulation::copyFactory () const {
    return new EliminationSequenceTriangulation(*this);
}
}  //namespace gum

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; space-indent on; indent-width 0;
