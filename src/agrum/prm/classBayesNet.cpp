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
/**
 * @file
 * @brief Implementation of ClassBayesNet.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/classBayesNet.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/classBayesNet.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

void
ClassBayesNet::__init(const Class& c) {
  for (DAG::NodeIterator node = c.dag().beginNodes(); node != c.dag().endNodes(); ++node) {
    try {
      // Adding the attribute
      if (ClassElement::isAttribute(c.get(*node)) or ClassElement::isAggregate(c.get(*node))) {
        const ClassElement& elt = c.get(*node);
        __dag.insertNode(elt.id());
        __varNodeMap.insert(&(elt.type().variable()), &elt);
      }
    } catch (NotFound&) {
      // Not an attribute
    }
  }
  for (ArcSet::iterator arc = c.dag().beginArcs(); arc != c.dag().endArcs(); ++arc) {
    try {
      __dag.insertArc(arc->tail(), arc->head());
    } catch (InvalidNode&) {
      // Not added means not an attribute
    }
  }
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
