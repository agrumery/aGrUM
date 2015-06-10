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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn an undirected graph structure
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/constraints/structuralConstraintUndiGraph.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintUndiGraph.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph() {
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
        unsigned int nb_nodes) {
      setGraph(nb_nodes);
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// constructor starting with a given graph
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
        const UndiGraph &graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// copy constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
        const StructuralConstraintUndiGraph &from)
        : _UndiGraph__graph(from._UndiGraph__graph) {
      GUM_CONS_CPY(StructuralConstraintUndiGraph);
    }

    /// move constructor
    StructuralConstraintUndiGraph::StructuralConstraintUndiGraph(
        StructuralConstraintUndiGraph &&from)
        : _UndiGraph__graph(std::move(from._UndiGraph__graph)) {
      GUM_CONS_MOV(StructuralConstraintUndiGraph);
    }

    /// destructor
    StructuralConstraintUndiGraph::~StructuralConstraintUndiGraph() {
      GUM_DESTRUCTOR(StructuralConstraintUndiGraph);
    }

    /// copy operator
    StructuralConstraintUndiGraph &StructuralConstraintUndiGraph::
    operator=(const StructuralConstraintUndiGraph &from) {
      if (this != &from) {
        _UndiGraph__graph = from._UndiGraph__graph;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintUndiGraph &StructuralConstraintUndiGraph::
    operator=(StructuralConstraintUndiGraph &&from) {
      if (this != &from) {
        _UndiGraph__graph = std::move(from._UndiGraph__graph);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
