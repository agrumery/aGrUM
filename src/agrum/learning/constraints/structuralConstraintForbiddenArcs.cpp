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
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/constraints/structuralConstraintForbiddenArcs.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintForbiddenArcs.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs() {
      GUM_CONSTRUCTOR(StructuralConstraintForbiddenArcs);
    }

    /// constructor starting with a given graph
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs(
        const DiGraph &graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintForbiddenArcs);
    }

    /// copy constructor
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs(
        const StructuralConstraintForbiddenArcs &from)
        : _ForbiddenArcs__forbidden_arcs(from._ForbiddenArcs__forbidden_arcs) {
      GUM_CONS_CPY(StructuralConstraintForbiddenArcs);
    }

    /// move constructor
    StructuralConstraintForbiddenArcs::StructuralConstraintForbiddenArcs(
        StructuralConstraintForbiddenArcs &&from)
        : _ForbiddenArcs__forbidden_arcs(
              std::move(from._ForbiddenArcs__forbidden_arcs)) {
      GUM_CONS_MOV(StructuralConstraintForbiddenArcs);
    }

    /// destructor
    StructuralConstraintForbiddenArcs::~StructuralConstraintForbiddenArcs() {
      GUM_DESTRUCTOR(StructuralConstraintForbiddenArcs);
    }

    /// copy operator
    StructuralConstraintForbiddenArcs &StructuralConstraintForbiddenArcs::
    operator=(const StructuralConstraintForbiddenArcs &from) {
      if (this != &from) {
        _ForbiddenArcs__forbidden_arcs = from._ForbiddenArcs__forbidden_arcs;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintForbiddenArcs &StructuralConstraintForbiddenArcs::
    operator=(StructuralConstraintForbiddenArcs &&from) {
      if (this != &from) {
        _ForbiddenArcs__forbidden_arcs =
            std::move(from._ForbiddenArcs__forbidden_arcs);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
