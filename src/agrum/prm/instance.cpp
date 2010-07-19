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
 * @brief Source implementation of gum::prm::Instance
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/instance.h>
#include <agrum/multidim/multiDimSparse.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/instance.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

Instance::Instance(const std::string& name, Class& type):
  PRMObject(name), __type(&type), __params(0)
{
  GUM_CONSTRUCTOR( Instance );
  // First we create attributes for each aggregate in type
  for (Set<Aggregate*>::iterator agg = __type->aggregates().begin(); agg != __type->aggregates().end(); ++agg) {
    __copyAggregates(*agg);
  }
  // We add attributes in type by reference for inner ones and by copy for output ones
  for (Set<Attribute*>::iterator attr = __type->attributes().begin(); attr != __type->attributes().end(); ++attr) {
    if (type.isOutputNode(**attr)) {
      __copyAttribute(*attr);
    } else {
      __nodeIdMap.insert((*attr)->id(), *attr);
      __bijection.insert(&((*attr)->type().variable()), &((*attr)->type().variable()));
    }
  }
  // Then we instantiate each parameters in type
  if (__type->parameters().size()) {
    __params = new Set<NodeId>();
  }
  for (Set<Attribute*>::iterator iter = __type->parameters().begin(); iter != __type->parameters().end(); ++iter) {
    __copyParameter(*iter);
  }
}

void
Instance::__copyParameter(Attribute* source) {
  Attribute* attr = 0;
  if (__type->isOutputNode(*source)) {
    attr = __nodeIdMap[source->id()];
  } else {
    attr = new Attribute(source->name(), source->type());
  }
  Instantiation i(attr->cpf()), j(source->cpf());
  for (i.setFirst(), j.setFirst(); not (i.end() or j.end()); i.inc(), j.inc()) {
    attr->cpf().set(i, source->cpf().get(j));
  }
  attr->setId(source->id());
  __nodeIdMap.insert(attr->id(), attr);
  __bijection.insert(&(attr->type().variable()), &(source->type().variable()));
  __trash.insert(attr);
}

Instance::~Instance() {
  GUM_DESTRUCTOR( Instance );
  for (Set<Attribute*>::iterator iter = __trash.begin(); iter != __trash.end(); ++iter) {
    delete *iter;
  }
  typedef Property< Set< Instance* >* >::onNodes::iterator TmpIterator;
  for (TmpIterator iter = __referenceMap.begin(); iter != __referenceMap.end(); ++iter) {
    delete *iter;
  }
  typedef Property< std::vector<pair>* >::onNodes::iterator Foo;
  for (Foo iter = __referingAttr.begin(); iter != __referingAttr.end(); ++iter) {
    delete *iter;
  }
  if (__params) {
    delete __params;
  }
}

void
Instance::instantiate() {
  // First retrieving any referenced instance
  for (Set<SlotChain*>::iterator chain = type().slotChains().begin(); chain != type().slotChains().end(); ++chain) {
    __instantiateSlotChain(*chain);
  }
  // Now we need to add referred instance to each input node
  // For Attributes we first add parents, then we initialize CPF
  for (Set<Attribute*>::iterator iter = type().attributes().begin(); iter != type().attributes().end(); ++iter) {
    if (not type().isParameter(**iter)) {
      if (isInstantiated((*iter)->id())) {
        __copyAttributeCPF(__nodeIdMap[(**iter).id()]);
      } else {
        for (DAG::ArcIterator arc = type().dag().parents((**iter).id()).begin();arc != type().dag().parents((**iter).id()).end(); ++arc) {
          if (isInstantiated(arc->tail())) {
            Attribute* old_attr = __nodeIdMap[(**iter).id()];
            Potential<prm_float>* pot = new Potential<prm_float>();
            pot->add(old_attr->type().variable());
            Attribute* new_attr = new Attribute(old_attr->name(), &(old_attr->type()), pot, false);
            __trash.insert(new_attr);
            new_attr->setId(old_attr->id());
            __nodeIdMap[new_attr->id()] = new_attr;
            __copyAttributeCPF(new_attr);
            break;
          }
        }
      }
    }
  }
  // For Aggregate we add parents
  for (Set<Aggregate*>::iterator iter = type().aggregates().begin(); iter != type().aggregates().end(); ++iter) {
    const ArcSet& parents = type().dag().parents((*iter)->id());
    Attribute& attr = get((**iter).safeName());
    for (ArcSet::const_iterator arc = parents.begin(); arc != parents.end(); ++arc) {
      try {
        attr.addParent(get(arc->tail()));
      } catch (NotFound&) {
        SlotChain& sc = static_cast<SlotChain&>(type().get(arc->tail()));
        const Set<Instance*>& instances = getInstances(sc.id());
        for (Set<Instance*>::iterator i = instances.begin(); i != instances.end(); ++i) {
          attr.addParent((*i)->get(sc.lastElt().safeName()));
        }
      }
    }
  }
}

void
Instance::__instantiateSlotChain(SlotChain* sc) {
  __referenceMap.insert(sc->id(), new Set<Instance*>());
  // An instantiated slot chain is a tree, to find all leaves we proceed with a deep run
  std::vector< std::pair<Instance*, Size> > stack;
  stack.push_back(std::pair<Instance*, Size>(this, 0));
  Set<Instance*> visited;
  // Last element is an attribute, and we only want the instance containing it
  Size depth_stop = sc->chain().size() -1;
  // Let's go!
  while (not stack.empty()) {
    Instance* current = stack.back().first;
    Size depth = stack.back().second;
    stack.pop_back();
    if (not visited.exists(current)) {
      visited.insert(current);
      if ( depth < depth_stop) {
        try {
          NodeId refId = sc->chain().atPos(depth)->id();
          Set<Instance*>* instances = current->__referenceMap[refId];
          for (Set<Instance*>::iterator iter = instances->begin(); iter != instances->end(); ++iter)
            stack.push_back(std::make_pair(*iter, depth + 1));
        } catch (NotFound&) {
          GUM_ERROR(NotFound, "found an uninstantiated reference");
        }
      } else {
        __referenceMap[sc->id()]->insert(current);
        __addReferingInstance(sc, current);
        // If slot chain is single, it could be used by an attribute so we add the corresponding DiscreteVariable
        // to __bijection for CPF initialisation
        if (not sc->isMultiple()) {
          __bijection.insert(&(current->get(sc->lastElt().safeName()).type().variable()), &(sc->type().variable()));
        }
      }
    }
  }
}

void
Instance::__copyAttributeCPF(Attribute* attr) {
  const MultiDimImplementation<prm_float>* impl = type().get(attr->safeName()).cpf().getContent();
  delete (attr->__cpf);
  attr->__cpf = 0;
  if (dynamic_cast< const MultiDimReadOnly<prm_float>* >(impl)) {
    if (dynamic_cast< const MultiDimAggregator<prm_float>* >(impl)) {
      attr->__cpf = new Potential<prm_float>(static_cast<MultiDimImplementation<prm_float>*>(impl->newFactory()));
      attr->__cpf->add(attr->type().variable());
      for (ArcSet::iterator arc = type().dag().parents(attr->id()).begin(); arc != type().dag().parents(attr->id()).end(); ++arc) {
        attr->addParent(get(arc->tail()));
      }
    } else if (dynamic_cast< const MultiDimNoisyORCompound<prm_float>* >(impl)) {
      attr->__cpf = new Potential<prm_float>(new MultiDimNoisyORCompound<prm_float>(__bijection, static_cast<const MultiDimNoisyORCompound<prm_float>&>(*impl)));
    } else if (dynamic_cast< const MultiDimNoisyORNet<prm_float>* >(impl)) {
      attr->__cpf = new Potential<prm_float>(new MultiDimNoisyORNet<prm_float>(__bijection, static_cast<const MultiDimNoisyORNet<prm_float>&>(*impl)));
    } else {
      GUM_ERROR(FatalError, "encountered an unexpected MultiDim implementation");
    }
  } else {
    delete (attr->__cpf);
    if (dynamic_cast< const MultiDimArray<prm_float>* >(impl)) {
      attr->__cpf = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bijection(), static_cast< const MultiDimArray<prm_float>& >(*impl)));
    } else if (dynamic_cast< const MultiDimSparse<prm_float>* >(impl)) {
      // Copy the sparse array in a MultiDimArray, this is sloppy but fast
      // Still there is no real use of MultiDimSparse so this is not really a problem
      MultiDimArray<prm_float>* array = new MultiDimArray<prm_float>();
      for (MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter) {
        array->add(**iter);
      }
      for (Instantiation i(*impl); not i.end(); i.inc()) {
        array->set(i, impl->get(i));
      }
      attr->__cpf = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bijection(), *array));
    } else {
      // Just need to make the copy using the bijection but we only use multidim array
      GUM_ERROR(FatalError, "encountered an unexpected MultiDim implementation");
    }
  }
}

void
Instance::add(NodeId id, Instance& instance) {
  ClassElement* elt = 0;
  try {
    elt = &(type().get(id));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ClassElement matches the given id");
  }
  switch (elt->elt_type()) {
    case ClassElement::prm_refslot:
      {
        ReferenceSlot* ref = static_cast<ReferenceSlot*>(elt);
        // Checking if instance's type is legal
        if (not instance.type().isSubTypeOf(ref->slotType())) {
          GUM_ERROR(TypeError, "given Instance type is not a proper "
              "subclass of the ReferenceSlot slot type");
        }
        // Checking the reference's size limit
        if ( __referenceMap.exists(id) and
            (not static_cast<ReferenceSlot&>(type().get(id)).isArray()) and
            (__referenceMap[id]->size() == 1) )
        {
          GUM_ERROR(OutOfUpperBound, "ReferenceSlot size limit reached");
        }
        break;
      }
    case ClassElement::prm_slotchain:
      {
        SlotChain& sc = static_cast<SlotChain&>(type().get(id));
        // Checking if instance's type is legal
        if (not instance.type().isSubTypeOf(sc.end())) {
          GUM_ERROR(TypeError, "given Instance type is not a proper "
              "subclass of the ClassElementContainer pointed"
              " by the SlotChain");
        }
        // Checking the reference's size limit
        if ( __referenceMap.exists(id) and
            (not static_cast<SlotChain&>(type().get(id)).isMultiple()) and
            (__referenceMap[id]->size() == 1) )
        {
          GUM_ERROR(OutOfUpperBound, "SlotChain size limit reached");
        }
        // We need to plug this to the referred instance.
        GUM_CHECKPOINT;
        break;
      }
    default: {
               if (not type().isOutputNode(*elt)) {
                 GUM_ERROR(WrongClassElement, "given ClassElement is not an output node");
               }
             }
  }
  if (not __referenceMap.exists(id)) {
    __referenceMap.insert(id, new Set<Instance*>());
  }
  __referenceMap[id]->insert(&instance);
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
