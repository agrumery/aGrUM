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
 * @brief Source implementation of gum::Instance
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/instance.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/instance.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

Instance::Instance(const std::string& name, Class& type):
  ClassElementContainer(name, type, true), __bijection(0)
{
  GUM_CONSTRUCTOR( Instance );
}

Instance::~Instance()
{
  GUM_DESTRUCTOR( Instance );
  // Deleting set of instances (but not the instances)
  typedef Property< Set< Instance* >* >::onNodes::iterator set_iterator;
  for (set_iterator iter = __referenceMap.begin(); iter != __referenceMap.end(); ++iter) {
    (*iter)->clear();
    delete *iter;
  }
  typedef Property< INMap* >::onNodes::iterator map_iterator;
  for (map_iterator iter = __slotChainMap.begin(); iter != __slotChainMap.end(); ++iter) {
    delete *iter;
  }
  // Deleting set of inverse slot chains
  typedef Property< Set< InverseSC* >* >::onNodes::iterator PropIter;
  for (PropIter set = __inverseSC.begin(); set != __inverseSC.end(); ++set) {
    for (Set< InverseSC* >::iterator iter = (*set)->begin(); iter != (*set)->end(); ++iter) {
      delete *iter;
    }
    delete *set;
  }
  delete __bijection;
}

Instance::Instance(const Instance& from):
  ClassElementContainer(from)
{
  GUM_CONS_CPY( Instance );
  GUM_ERROR(FatalError, "illegal call of gum::Instance copy constructor.");
}

Instance&
Instance::operator=(const Class& from)
{
  GUM_ERROR(FatalError, "illegal call of gum::Instance copy operator.");
}

void
Instance::add(NodeId id, Instance& instance)
{
  if (get(id).elt_type() == ClassElement::prm_refslot) {
    const ReferenceSlot& ref = static_cast<const ReferenceSlot&>(get(id));
    if (instance.type().isSubTypeOf(ref.slotType())) {
      GUM_ERROR(OperationNotAllowed, "illegal instance type for this reference slot.");
    }
    if (not __referenceMap.exists(id)) {
      __referenceMap.insert(id, new Set< Instance* >());
      __referenceMap[id]->insert(&instance);
    } else if (ref.isArray()) {
      __referenceMap[id]->insert(&instance);
    } else {
      GUM_ERROR(OperationNotAllowed, "This single reference has been already instantiated.");
    }
  } else {
    GUM_ERROR(NotFound, "No gum::ReferenceSlot with the given id.");
  }
}

void
Instance::instantiate(NodeId id)
{
  if (not type().instantiations().exists(id)) {
    GUM_ERROR(OperationNotAllowed, "can't instantiate this node.");
  } else if (not __instantiated_nodes.contains(id)) {
    __instantiated_nodes.insert(id);
    // first instantiate parents if necessary
    for (DAG::ArcIterator parent = parents(id).begin(); parent != parents(id).end(); ++parent) {
      __instantiateParent(id, parent->tail());
    }
    // Since the model can contains loop (but not cycle) point this node can be already instantiated
    if (not _exists(id)) {
      // second we instantiate the node
      switch (type().get(id).elt_type()) {
        case ClassElement::prm_attribute: {
                                            __instantiateAttribute(id);
                                            break;
                                          }
        case ClassElement::prm_aggregate: {
                                            __instantiateAggregate(id);
                                            break;
                                          }
        default: {
                   GUM_ERROR(FatalError, "invalid context to instantiate given node.");
                 }
      }
      // third tell to this node's children to instantiate themselves
      for (DAG::ArcIterator child = children(id).begin(); child != children(id).end(); ++child) {
        __instantiateChildren(child->head(), id);
      }
    }
  }
}

void
Instance::__instantiateParent(NodeId child, NodeId parent) {
  switch (type().get(parent).elt_type()) {
    case ClassElement::prm_slotchain: {
                                        if (not __slotChainMap.exists(parent)) {
                                          __slotChainMap.insert(parent, new Instance::INMap());
                                          __instantiateSlotChain(parent);
                                        }
                                        break;
                                      }
    case ClassElement::prm_attribute:
    case ClassElement::prm_aggregate: {
                                        try {
                                          instantiate(parent);
                                        } catch (OperationNotAllowed&) {
                                          // it's ok, parent don't need to be instantiated
                                        }
                                        break;
                                      }
    default: {
               GUM_ERROR(FatalError, "can't instantiate this node.");
             }
  }
}

void
Instance::__instantiateAttribute(NodeId id) {
  Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
  Type* t = new Type(type().get(id).type());
  bijection.insert(&(t->variable()), &(type().get(id).type().variable()));
  for (DAG::ArcIterator parent = parents(id).begin(); parent != parents(id).end(); ++parent) {
    switch (get(parent->tail()).elt_type()) {
      case ClassElement::prm_attribute: {
                                          bijection.insert(&(get(parent->tail()).type().variable()),
                                                           &(type().get(parent->tail()).type().variable()));
                                          break;
                                        }
      case ClassElement::prm_slotchain: {
                                          const SlotChain& sc = static_cast<const SlotChain&>(get(parent->tail()));
                                          bijection.insert(&(getInstance(sc.id()).get(sc.lastElt().id()).type().variable()),
                                                           &(sc.lastElt().type().variable()));
                                          break;
                                        }
      default: {
                 GUM_ERROR(FatalError, "invalid ClassElement type as parent.");
                 break;
               }
    }
  }
  const MultiDimArray<prm_float>* array
    = dynamic_cast<const MultiDimArray<prm_float>*>(const_cast<const Potential<prm_float>&>(type().get(id).cpf()).getContent());
  if (array != 0) {
    Potential<prm_float>* cpf = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bijection, *array));
    Attribute* attr = new Attribute(type().get(id).name(), t, cpf);
    _add(attr, id);
  } else {
    GUM_ERROR(FatalError, "Unhandle Potential implementation.");
  }
}

void
Instance::__instantiateAggregate(NodeId id) {
  Attribute* attr = new Attribute(type().get(id).name(), type().get(id).type(), static_cast<Aggregate&>(type().get(id)).buildImpl());
  for (DAG::ArcIterator parent = parents(id).begin(); parent != parents(id).end(); ++parent) {
    switch (get(parent->tail()).elt_type()) {
      case ClassElement::prm_slotchain: {
                                          SlotChain& sc = static_cast<SlotChain&>(get(parent->tail()));
                                          for (Set< Instance* >::iterator i = __referenceMap[parent->tail()]->begin(); i != __referenceMap[parent->tail()]->end(); ++i) {
                                            attr->addParent((**i).get(sc.lastElt().id()));
                                          }
                                          break;
                                        }
      case ClassElement::prm_attribute: {
                                          attr->addParent(get(parent->tail()));
                                          break;
                                        }
      default: {
                 GUM_ERROR(FatalError, "invalid context to instantiate aggregate.");
               }
    }
  }
  _add(attr, id);
}

void
Instance::__instantiateChildren(NodeId child, NodeId parent)
{
  // check if this needs to be instantiated
  if (type().instantiations().exists(child) and (not __instantiated_nodes.contains(child))) {
    instantiate(child);
  } else {
    // check if other parents of child to instantiate them if necessary
    for (DAG::ArcIterator parent = parents(child).begin(); parent != parents(child).end(); ++parent) {
      try {
        instantiate(parent->tail());
      } catch (OperationNotAllowed&) {
        // it's ok, parent don't need to be instantiated
      }
    }
    // Since the model can contains loop (but not cycle) point this node can be already instantiated
    if (not _exists(child)) {
      MultiDimBijArray<prm_float>::VarBijection bijection;
      for (DAG::ArcIterator parent = parents(child).begin(); parent != parents(child).end(); ++parent) {
        if (__instantiated_nodes.contains(parent->tail())) {
          switch (get(parent->tail()).elt_type()) {
            case ClassElement::prm_attribute: {
                                                bijection.insert(&(get(parent->tail()).type().variable()),
                                                    &(type().get(parent->tail()).type().variable()));
                                                break;
                                              }
            case ClassElement::prm_slotchain: {
                                                const SlotChain& sc = static_cast<const SlotChain&>(get(parent->tail()));
                                                bijection.insert(&(getInstance(sc.id()).get(sc.lastElt().id()).type().variable()),
                                                    &(sc.lastElt().type().variable()));
                                                break;
                                              }
            default: {
                       GUM_ERROR(FatalError, "invalid ClassElement type as parent.");
                       break;
                     }
          }
        }
      }
      if (not bijection.empty()) {
        // Adding instantiated parent's variables
        const Potential<prm_float>& cpf = type().get(child).cpf();
        const MultiDimArray<prm_float>* array = dynamic_cast<const MultiDimArray<prm_float>*>(cpf.getContent());
        if (array != 0) {
          Potential<prm_float>* p = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bijection, *array));
          Attribute* attr = new Attribute(type().get(child).name(), type().get(child).type(), p);
          _add(attr, child);
        } else {
          GUM_ERROR(OperationNotAllowed, "Unusable MultiDimImplementation.");
        }
      }
    }
  }
}

void
Instance::__instantiateSlotChain(NodeId id)
{
  SlotChain& chain = static_cast<SlotChain&>(get(id));
  // If there is type specialisation we need to know which type to use
  const Type& type = chain.type();
  std::stringstream cast;
  cast << "<" << type.name() << ">" << chain.lastElt().name();
  // An instantiated slot chain is a tree, to find all leaves we proceed
  // with a deep run.
  std::vector< std::pair<Instance*, Size> > stack;
  stack.push_back(std::pair<Instance*, Size>(this, 0));
  Set<Instance*> visited;
  // Last element is an attribute, and we only want the instance
  // containing it.
  Size depth_stop = chain.chain().size();
  // Loop variables
  NodeId lastElt_id = 0;
  Size depth = 0;
  Instance* current = 0;
  Set<Instance*>* instances = 0;
  // Let's go!
  while (not stack.empty()) {
    current = stack.back().first;
    depth = stack.back().second;
    stack.pop_back();
    if (not visited.exists(current)) {
      visited.insert(current);
      if ( depth < depth_stop) {
        try {
          instances = current->__referenceMap[chain.chain().atPos(depth)->id()];
          for (Set<Instance*>::iterator iter = instances->begin();
              iter != instances->end(); ++iter) {
            stack.push_back(std::make_pair(*iter, depth + 1));
          }
        } catch (NotFound&) {
          GUM_ERROR(NotFound, "Uninstantiated reference.");
        }
      } else {
        lastElt_id = current->get(chain.id()).type() == type?
          chain.id():current->get(cast.str()).id();
        __slotChainMap[chain.id()]->insert(current, lastElt_id);
        current->instantiate(lastElt_id);
        current->__addAsInverseSC(lastElt_id, this, id);
      }
    }
  }
}

void
Instance::__addAsInverseSC(NodeId id, Instance* i, NodeId sc_id) {
  Instance::InverseSC* inv = new Instance::InverseSC(i, sc_id);
  // try {
    try {
      __inverseSC[id]->insert(inv);
    } catch (NotFound&) {
      __inverseSC.insert(id, new Set< Instance::InverseSC* >());
      __inverseSC[id]->insert(inv);
    }
  // } catch (DuplicateElement&) {
  //   // That could happen but will trace it in debug mode just in case
  // }
}

void
Instance::__init_bijection() const {
  __bijection = new Bijection<const DiscreteVariable*, const DiscreteVariable*>();
  for (Set<Attribute*>::iterator attr = attributes().begin();
       attr != attributes().end(); ++attr) {
    __bijection->insert(&((**attr).type().variable()),
                        &(type().get((**attr).id()).type().variable()));
  }
  for (Set<SlotChain*>::iterator sc = slotChains().begin();
       sc != slotChains().end(); ++sc) {
    if (not (**sc).isMultiple()) {
      __bijection->insert(&(getInstance((**sc).id()).get((**sc).lastElt().id()).type().variable()),
                          &((**sc).lastElt().type().variable()));
    }
  }
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
