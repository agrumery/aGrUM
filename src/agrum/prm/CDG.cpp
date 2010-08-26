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
 * @brief Implementation of CDG.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/CDG.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/CDG.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

// Destructor.
CDG::~CDG()
{
  GUM_DESTRUCTOR( CDG );
  for (NodeMap::iterator iter = __node_map.begin(); iter != __node_map.end(); ++iter) {
    delete *iter;
  }
  typedef Property< EltPair* >::onNodes::iterator EltMapIterator;
  for (EltMapIterator iter = __elt_map.begin(); iter != __elt_map.end(); ++iter) {
    delete *iter;
  }
}

// Build the class dependency graph.
void
CDG::__buildGraph(const PRM& prm) {
  // First we add all nodes
  for (Set<Class*>::const_iterator iter = prm.classes().begin(); iter != prm.classes().end(); ++iter) {
    __node_map.insert(*iter, new HashTable<const ClassElement*, NodeId>());
    for (DAG::NodeIterator jter = (*iter)->dag().beginNodes(); jter != (*iter)->dag().endNodes(); ++jter)
      __addNode(*iter, (*iter)->get(*jter));
  }
  for (Set<Interface*>::const_iterator iter = prm.interfaces().begin(); iter != prm.interfaces().end(); ++iter) {
    __node_map.insert(*iter, new HashTable<const ClassElement*, NodeId>());
    for (DAG::NodeIterator jter = (*iter)->dag().beginNodes(); jter != (*iter)->dag().endNodes(); ++jter)
      __addNode(*iter, (*iter)->get(*jter));
  }
  // Then we add the arcs
  for (Set<Class*>::const_iterator iter = prm.classes().begin(); iter != prm.classes().end(); ++iter)
    for (DAG::NodeIterator jter = (*iter)->dag().beginNodes(); jter != (*iter)->dag().endNodes(); ++jter)
      __addArcs(**iter, *jter, *(__node_map[*iter]));
}

// Add arcs in __graph.
void
CDG::__addArcs(const ClassElementContainer& c, NodeId node,
               HashTable<const ClassElement*, NodeId>& map)
{
  switch (c.get(node).elt_type()) {
    case ClassElement::prm_slotchain:
      {
        const SlotChain& sc = static_cast<const SlotChain&>(c.get(node));
        for (DAG::ArcIterator arc = c.dag().children(node).begin(); arc != c.dag().children(node).end(); ++arc)
          __graph.insertArc((*(__node_map[&(sc.end())]))[&(sc.end().get(sc.lastElt().safeName()))], map[&(c.get(arc->head()))]);
        break;
      }
    case ClassElement::prm_aggregate:
    case ClassElement::prm_attribute:
      {
        for (DAG::ArcIterator arc = c.dag().children(node).begin(); arc != c.dag().children(node).end(); ++arc)
          __graph.insertArc(map[&(c.get(node))], map[&(c.get(arc->head()))]);
        break;
      }
    default: { /* do nothing */ break; }
  }
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
