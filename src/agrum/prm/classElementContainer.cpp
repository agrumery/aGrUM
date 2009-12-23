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
 * @brief Source implementation of gum::ClassElementContainer
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/classElementContainer.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/classElementContainer.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

ClassElementContainer::ClassElementContainer(const std::string& name):
  PRMObject(name), __alternate(0), __delegateDAG(false)
{
  GUM_CONSTRUCTOR( ClassElementContainer );
  __dag = new DAG();
}

ClassElementContainer::ClassElementContainer(const std::string& name,
                                             ClassElementContainer& alternate,
                                             bool delegateDAG):
  PRMObject(name), __alternate(&alternate), __delegateDAG(delegateDAG)
{
  GUM_CONSTRUCTOR( ClassElementContainer );
  if (delegateDAG) {
    __dag = alternate.__dag;
  } else {
    __dag = new DAG(* (alternate.__dag));
  }
  {
    typedef Set<Attribute*>::iterator SetIter;
    for (SetIter iter = alternate.__attributes.begin();
         iter != alternate.__attributes.end(); ++iter) {
      __attributes.insert(*iter);
    }
  }
  {
    typedef Set<Aggregate*>::iterator SetIter;
    for (SetIter iter = alternate.__aggregates.begin();
         iter != alternate.__aggregates.end(); ++iter) {
      __aggregates.insert(*iter);
    }
  }
  {
    typedef Set<ReferenceSlot*>::iterator SetIter;
    for (SetIter iter = alternate.__referenceSlots.begin();
         iter != alternate.__referenceSlots.end(); ++iter) {
      __referenceSlots.insert(*iter);
    }
  }
  {
    typedef Set<SlotChain*>::iterator SetIter;
    for (SetIter iter = alternate.__slotChains.begin();
         iter != alternate.__slotChains.end(); ++iter) {
      __slotChains.insert(*iter);
    }
  }
}

ClassElementContainer::~ClassElementContainer()
{
  GUM_DESTRUCTOR( ClassElementContainer );

  __attributes.clear();
  __referenceSlots.clear();
  __aggregates.clear();
  __nameMap.clear();

  // Deleting __nodeIdMap
  typedef Property< ClassElement* >::onNodes::iterator node_iterator;
  for (node_iterator iter = __nodeIdMap.begin(); iter != __nodeIdMap.end(); ++iter) {
    delete *iter;
  }
  if (not __delegateDAG) {
    delete __dag;
  }
}

ClassElementContainer&
ClassElementContainer::operator=(const ClassElementContainer& source)
{
  GUM_ERROR(FatalError, "illegal call to gum::ClassElementContainer copy operator.");
}

ClassElementContainer::ClassElementContainer(const ClassElementContainer& source):
  PRMObject(source), __alternate(source.__alternate), __delegateDAG(source.__delegateDAG)
{
  GUM_CONS_CPY( ClassElementContainer );
  GUM_ERROR(FatalError, "illegal call to gum::ClassElementContainer copy operator.");
}

std::ostream&
operator<<(std::ostream& output, const ClassElementContainer& container)
{
  std::string tab = "  ";
  output << "digraph \"" << container.name() << "\" {" << std::endl;
  for (DAG::NodeIterator node_iter = container.dag().beginNodes();
       node_iter != container.dag().endNodes(); ++node_iter) {
    if (container.dag().children(*node_iter).size() > 0) {
      for (DAG::ArcIterator child_iter = container.dag().children(*node_iter).begin();
          child_iter != container.dag().children(*node_iter).end(); ++child_iter) {
        output << tab << "\"" << container.get(*node_iter).name() << "\" -> "
               << "\"" << container.get(child_iter->head()).name() << "\";" << std::endl;
      }
    } else if (container.dag().parents(*node_iter).size() == 0) {
      output << tab << "\"" << container.get(*node_iter).name() << "\";" << std::endl;
    }
  }
  output << "}" << std::endl;
  return output;
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
