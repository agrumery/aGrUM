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
 * @brief Implementation of StructuredBayesBall.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/structuredBayesBall.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/structuredBayesBall.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

StructuredBayesBall::~StructuredBayesBall() {
  GUM_DESTRUCTOR( StructuredBayesBall );
  typedef HashTable<std::string, std::pair< Set<NodeId>*, Size> >::iterator Iter;
  for (Iter iter = __reqMap.begin(); iter != __reqMap.end(); ++iter) {
    delete iter->first;
  }
}

void
StructuredBayesBall::__clean() {
  typedef HashTable<std::string, std::pair< Set<NodeId>*, Size> >::iterator Iter;
  for (Iter iter = __reqMap.begin(); iter != __reqMap.end(); ++iter) {
    delete iter->first;
  }
  __keyMap.clear();
  __reqMap.clear();
}

bool
StructuredBayesBall::__isHardEvidence(const Instance* i, NodeId n) {
  try {
    PRMInference::Chain chain = std::make_pair(i, &(i->get(n)));
    if (__inf->hasEvidence(chain)) {
      const Potential<prm_float>* e = __inf->evidence(i)[n];
      Instantiation inst(e);
      Size count = 0;
      for (inst.setFirst(); not inst.end(); inst.inc()) {
        if ( (e->get(inst) == (prm_float) 1) ) {
          ++count;
        } else if (e->get(inst) != (prm_float) 0) {
          return false;
        }
      }
      return (count == 1);
    }
    return false;
  } catch (NotFound&) {
    return false;
  }
}

void
StructuredBayesBall::__compute(const Instance* i, NodeId n) {
  __clean();
  /// Key = instance.ClassElement
  /// pair = <upper mark, lower mark>
  StructuredBayesBall::InstanceMap marks;
  __fromChild(i, n, marks);
  __fillMaps(marks);
  typedef StructuredBayesBall::InstanceMap::iterator Iter;
  for (Iter iter = marks.begin(); iter != marks.end(); ++iter) {
    delete *iter;
  }
}

void
StructuredBayesBall::__fromChild(const Instance* i, NodeId n, InstanceMap& marks) {
  if (not marks.exists(i)) {
    marks.insert(i, new StructuredBayesBall::MarkMap());
  }
  if (not marks[i]->exists(n)) {
    marks[i]->insert(n, std::pair<bool, bool>(false, false));
  }
  // Sending message to parents
  switch (i->type().get(n).elt_type()) {
    case ClassElement::prm_slotchain:
      {
        if (not __getMark(marks, i, n).first) {
          __getMark(marks, i, n).first = true;
          for (Set<Instance*>::iterator iter = i->getInstances(n).begin(); iter != i->getInstances(n).end(); ++iter) {
            NodeId id = (**iter).get(__getSC(i,n).lastElt().safeName()).id();
            __fromChild(*iter, id, marks);
          }
        }
        if (not __getMark(marks, i, n).second) {
          __getMark(marks, i, n).second = true;
          for (ArcSetIterator child = i->type().dag().children(n).begin(); child != i->type().dag().children(n).end(); ++child) {
            __fromParent(i, child->head(), marks);
          }
        }
        break;
      }
    case ClassElement::prm_aggregate:
    case ClassElement::prm_attribute:
      {
        if ( (not __getMark(marks, i, n).first) and (not __isHardEvidence(i, n)) ) {
          __getMark(marks, i, n).first = true;
          for (ArcSetIterator prnt = i->type().dag().parents(n).begin(); prnt != i->type().dag().parents(n).end(); ++prnt) {
            __fromChild(i, prnt->tail(), marks);
          }
        }
        if (not __getMark(marks, i, n).second) {
          __getMark(marks, i, n).second = true;
          // In i.
          for (ArcSetIterator child = i->type().dag().children(n).begin(); child != i->type().dag().children(n).end(); ++child) {
            __fromParent(i, child->head(), marks);
          }
          // Out of i.
          try {
            typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
            for (Iter iter = i->getRefAttr(n).begin(); iter != i->getRefAttr(n).end(); ++iter) {
              __fromParent(iter->first, iter->first->get(iter->second).id(), marks);
            }
          } catch (NotFound&) {
            // Not an inverse sc
          }
        }
        break;
      }
    default:
      {
        // We shouldn't reach any other ClassElement than Attribute or SlotChain.
        GUM_ERROR(FatalError, "This case is impossible.");
      }
  }
}

void
StructuredBayesBall::__fromParent(const Instance* i, NodeId n, InstanceMap& marks) {
  if (not marks.exists(i)) {
    marks.insert(i, new StructuredBayesBall::MarkMap());
  }
  if (not marks[i]->exists(n)) {
    marks[i]->insert(n, std::pair<bool, bool>(false, false));
  }
  // Concerns only Attribute (because of the hard evidence)
  if ( (__isHardEvidence(i, n)) and (not __getMark(marks, i, n).first) ) {
    __getMark(marks, i, n).first = true;
    for (ArcSetIterator iter = i->type().dag().parents(n).begin(); iter != i->type().dag().parents(n).end(); ++iter) {
      __fromChild(i, iter->tail(), marks);
    }
  } else if (not __getMark(marks, i, n).second) {
    __getMark(marks, i, n).second = true;
    // In i.
    for (ArcSetIterator iter = i->type().dag().children(n).begin(); iter != i->type().dag().children(n).end(); ++iter) {
      __fromParent(i, iter->head(), marks);
    }
    // Out of i.
    try {
      typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
      for (Iter iter = i->getRefAttr(n).begin(); iter != i->getRefAttr(n).end(); ++iter) {
        __fromParent(iter->first, iter->first->get(iter->second).id(), marks);
      }
    } catch (NotFound&) {
      // Not an inverse sc
    }
  }
}

void
StructuredBayesBall::__fillMaps(InstanceMap& marks) {
  // First find for each instance it's requisite nodes
  HashTable<const Instance*, Set<NodeId>*> req_map;
  for (StructuredBayesBall::InstanceMap::iterator iter = marks.begin(); iter != marks.end(); ++iter) {
    Set<NodeId>* req_set = new Set<NodeId>();
    for (StructuredBayesBall::MarkMap::iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
      if (jter->first) {
        req_set->insert(jter.key());
      }
    }
    req_map.insert(iter.key(), req_set);
  }
  // Remove all instances with 0 requisite nodes
  Set<const Instance*> to_remove;
  for (HashTable<const Instance*, Set<NodeId>*>::iterator iter = req_map.begin(); iter != req_map.end(); ++iter) {
    if ((**iter).size() == 0) {
      to_remove.insert(iter.key());
    }
  }
  for (Set<const Instance*>::iterator iter = to_remove.begin(); iter != to_remove.end(); ++iter) {
    delete req_map[*iter];
    req_map.erase(*iter);
  }
  // Fill __reqMap and __keyMap
  for (HashTable<const Instance*, Set<NodeId>*>::iterator iter = req_map.begin(); iter != req_map.end(); ++iter) {
    std::string key = __buildHashKey(iter.key(), **iter);
    if (__reqMap.exists(key)) {
      __keyMap.insert(iter.key(), std::pair<std::string, Set<NodeId>* >(key, __reqMap[key].first));
      __reqMap[key].second += 1;
      delete *iter;
      req_map[iter.key()] = 0;
    } else {
      __reqMap.insert(key, std::pair< Set<NodeId>*, Size>(*iter, 1));
      __keyMap.insert(iter.key(), std::pair<std::string, Set<NodeId>* >(key, *iter));
    }
  }
}

std::string
StructuredBayesBall::__buildHashKey(const Instance* i, Set<NodeId>& req_nodes) {
  std::stringstream sBuff;
  sBuff << i->type().name();
  for (DAG::NodeIterator node = i->type().dag().beginNodes(); node != i->type().dag().endNodes(); ++node) {
    if (req_nodes.exists(*node)) {
      sBuff << "-" << *node;
    }
  }
  return sBuff.str();
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
