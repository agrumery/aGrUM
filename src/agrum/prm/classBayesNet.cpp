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
        NodeId id = __dag.insertNode();
        __varNodeMap.insert(id, elt.type().variable());
        __varNodeMap.changeName(id, elt.safeName());
        // Adding in-going arcs of this ClassElement
        const ArcSet* set = &(c.dag().parents(*node));
        for (ArcSet::iterator arc = set->begin(); arc != set->end(); ++arc) {
          try {
            __dag.insertArc(__varNodeMap.idFromName(c.get(arc->tail()).safeName()), id);
          } catch (NotFound&) {
            // Either not an Attribtue / Aggregate or not yet added
          }
        }
        // Adding out-going arcs of this ClassElement
        set = &(c.dag().children(*node));
        for (ArcSet::iterator arc = set->begin(); arc != set->end(); ++arc) {
          try {
            __dag.insertArc(id, __varNodeMap.idFromName(c.get(arc->head()).safeName()));
          } catch (NotFound&) {
            // Either not an Attribtue / Aggregate or not yet added
          }
        }
      }
    } catch (NotFound&) {
      // Not an attribute
    }
  }
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
