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
 * @brief the structural constraint indicating that some arcs shall never be
 * removed or reversed
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/constraints/structuralConstraintMandatoryArcs.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintMandatoryArcs.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs() {
      GUM_CONSTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// constructor starting with a given graph
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
        const DiGraph &graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// copy constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
        const StructuralConstraintMandatoryArcs &from)
        : _MandatoryArcs__mandatory_arcs(from._MandatoryArcs__mandatory_arcs) {
      GUM_CONS_CPY(StructuralConstraintMandatoryArcs);
    }

    /// move constructor
    StructuralConstraintMandatoryArcs::StructuralConstraintMandatoryArcs(
        StructuralConstraintMandatoryArcs &&from)
        : _MandatoryArcs__mandatory_arcs(
              std::move(from._MandatoryArcs__mandatory_arcs)) {
      GUM_CONS_MOV(StructuralConstraintMandatoryArcs);
    }

    /// destructor
    StructuralConstraintMandatoryArcs::~StructuralConstraintMandatoryArcs() {
      GUM_DESTRUCTOR(StructuralConstraintMandatoryArcs);
    }

    /// copy operator
    StructuralConstraintMandatoryArcs &StructuralConstraintMandatoryArcs::
    operator=(const StructuralConstraintMandatoryArcs &from) {
      if (this != &from) {
        _MandatoryArcs__mandatory_arcs = from._MandatoryArcs__mandatory_arcs;
      }
      return *this;
    }

    /// move operator
    StructuralConstraintMandatoryArcs &StructuralConstraintMandatoryArcs::
    operator=(StructuralConstraintMandatoryArcs &&from) {
      if (this != &from) {
        _MandatoryArcs__mandatory_arcs =
            std::move(from._MandatoryArcs__mandatory_arcs);
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */
