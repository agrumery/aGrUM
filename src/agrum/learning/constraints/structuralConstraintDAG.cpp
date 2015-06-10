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
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/constraints/structuralConstraintDAG.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintDAG.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintDAG::StructuralConstraintDAG() {
      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintDAG::StructuralConstraintDAG(unsigned int nb_nodes)
        : StructuralConstraintDiGraph(nb_nodes) {
      StructuralConstraintDiGraph::setGraph(nb_nodes);

      DAG g;
      for (unsigned int i = 0; i < nb_nodes; ++i) {
        g.addNode(i);
      }
      _DAG__cycle_detector.setDAG(g);

      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// constructor starting with a given graph
    StructuralConstraintDAG::StructuralConstraintDAG(const DAG &graph) {
      StructuralConstraintDiGraph::setGraph(graph);
      _DAG__cycle_detector.setDAG(graph);

      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// copy constructor
    StructuralConstraintDAG::StructuralConstraintDAG(
        const StructuralConstraintDAG &from)
        : StructuralConstraintDiGraph(from),
          _DAG__cycle_detector(from._DAG__cycle_detector) {
      GUM_CONS_CPY(StructuralConstraintDAG);
    }

    /// move constructor
    StructuralConstraintDAG::StructuralConstraintDAG(StructuralConstraintDAG &&from)
        : StructuralConstraintDiGraph(std::move(from)),
          _DAG__cycle_detector(std::move(from._DAG__cycle_detector)) {
      GUM_CONS_MOV(StructuralConstraintDAG);
    }

    /// destructor
    StructuralConstraintDAG::~StructuralConstraintDAG() {
      GUM_DESTRUCTOR(StructuralConstraintDAG);
    }

    /// copy operator
    StructuralConstraintDAG &StructuralConstraintDAG::
    operator=(const StructuralConstraintDAG &from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(from);
        _DAG__cycle_detector = from._DAG__cycle_detector;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintDAG &StructuralConstraintDAG::
    operator=(StructuralConstraintDAG &&from) {
      if (this != &from) {
        StructuralConstraintDiGraph::operator=(std::move(from));
        _DAG__cycle_detector = std::move(from._DAG__cycle_detector);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
