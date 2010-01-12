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
 * @brief Implementation of PRMFactory.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/PRMFactory.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/PRMFactory.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

PRMFactory::PRMFactory()
{
  GUM_CONSTRUCTOR( PRMFactory );
  __prm = new PRM();
}

PRMFactory::~PRMFactory()
{
  GUM_DESTRUCTOR( PRMFactory );
}

void
PRMFactory::startClass(const std::string& name, const std::string& extends,
                       const Set<std::string>* implements)
{
  std::string real_name = __addPrefix(name);
  __checkDuplicateName(real_name);
  Class* c = 0;
  Class* mother = 0;
  Set<Class*> impl;
  if (implements != 0) {
    for (Set<std::string>::iterator iter = implements->begin();
         iter != implements->end(); ++iter) {
      try {
        impl.insert(__prm->__interfaceMap[*iter]);
      } catch (NotFound&) {
        try {
          impl.insert(__prm->__interfaceMap[__addPrefix(*iter)]);
        } catch (NotFound&) {
          __throwNotDeclared(PRMObject::prm_interface, *iter);
        }
      }
    }
  }
  if (extends != "") {
    // Retrieving the mother class
    try {
      mother = __prm->__classMap[extends];
    } catch (NotFound&) {
      try {
      mother = __prm->__classMap[__addPrefix(extends)];
      } catch (NotFound&) {
        __throwNotDeclared(PRMObject::prm_class, extends);
      }
    }
  }
  if ((extends == "") and impl.empty()) {
    c = new Class(real_name);
  } else if ((extends != "") and impl.empty()) {
    c = new Class(real_name, *mother);
  } else if ((extends == "") and (not impl.empty())) {
    c = new Class(real_name, impl);
  } else if ((extends != "") and (not impl.empty())) {
    c = new Class(real_name, *mother, impl);
  }
  __prm->__classMap.insert(c->name(), c);
  __prm->__classes.insert(c);
  __stack.push_back(c);
}

void
PRMFactory::setRawCPFByLines(const std::vector<float>& array)
{
  Attribute* a = __checkStackAttr(1);
  __checkStackClass(2);
  __checkArraySize(a, array);
  if (a->cpf().domainSize() != array.size()) {
    std::stringstream sBuff;
    sBuff << "found domain size of " << array.size();
    sBuff << " expected " << a->cpf().domainSize();
    GUM_ERROR(OperationNotAllowed, sBuff.str());
  }
  std::vector<prm_float> ugly;
  for (size_t i = 0; i < array.size(); ++i) {
    ugly.push_back((prm_float) array[i]);
  }
  a->cpf().fillWith(ugly);
}

void
PRMFactory::setRawCPFByColumns(const std::vector<float>& array)
{
  Attribute* a = __checkStackAttr(1);
  if (a->cpf().nbrDim() == 1) {
    setRawCPFByLines(array);
  } else {
    __checkArraySize(a, array);
    Instantiation inst(a->cpf());
    Instantiation jnst;
    typedef Sequence<const DiscreteVariable*>::const_iterator Iterator;
    Size pos = a->cpf().variablesSequence().size();
    do {
      --pos;
      if (&(a->type().variable()) != a->cpf().variablesSequence().atPos(pos)) {
        jnst.add(*(a->cpf().variablesSequence().atPos(pos)));
      }
    } while(pos != 0);
    size_t idx = 0;
    for (pos = 0; pos < a->type()->domainSize(); ++pos) {
      inst.chgVal(a->type().variable(), pos);
      for (jnst.setFirst(); not jnst.end(); jnst.inc()) {
        inst.chgValIn(jnst);
        a->cpf().set(jnst, array[idx]);
        ++idx;
      }
    }
  }
}

void
PRMFactory::__checkArraySize(Attribute* a, const std::vector<float>& array) const {
  typedef Sequence<const DiscreteVariable*>::iterator Iterator;
  size_t size = 1;
  for (Iterator iter = a->cpf().variablesSequence().begin();
       iter != a->cpf().variablesSequence().end(); ++iter) {
    size *= (*iter)->domainSize();
  }
  if (size != array.size()) {
    GUM_ERROR(OperationNotAllowed, "illegal CPT size");
  }
}

void
PRMFactory::setCPFByRule(const std::vector<std::string>& parents,
                         const std::vector<float>& values)
{
  Attribute* a = __checkStackAttr(1);
  Instantiation inst(a->cpf());
  Instantiation jnst, knst;
  const DiscreteVariable* var = 0;
  Size pos = 0;
  bool found = false;
  for (size_t i = 0; i < parents.size(); ++i) {
    var = a->cpf().variablesSequence().atPos(1+i);
    if (parents[i] == "*") {
      knst.add(*var);
    } else {
      jnst.add(*var);
      pos = 0;
      found = false;
      for (Size j = 0; j < var->domainSize(); ++j) {
        if (var->label(j) == parents[i]) {
          jnst.chgVal(*var, j);
          found = true;
          break;
        }
      }
      if (not found) GUM_ERROR(NotFound, "no label with this name.");
    }
  }
  inst.chgValIn(jnst);
  for (Size i = 0; i < a->type()->domainSize(); ++i) {
    inst.chgVal(a->type().variable(), i);
    for (inst.setFirstIn(knst); not inst.end(); inst.incIn(knst)) {
      a->cpf().set(inst, values[i]);
    }
  }
}

void
PRMFactory::__checkAggType_or_and(Class* c, const std::vector<std::string>& chains,
                                            const std::vector<std::string>& params)
{
  Type* type = __retrieveType("boolean"); // The type on which the aggregator applies
  for (size_t i = 0; i < chains.size(); ++i) {
    ClassElement* elt = 0;
    try {
      elt = &(c->get(chains[i]));
    } catch (NotFound&) {
      // a new slot chain?
      c->add(__buildSlotChain(c, chains[i]));
      elt = &(c->get(chains[i]));
    }
    if ( (*type) != elt->type() ) {
      std::stringstream sBuff;
      sBuff << "slot chain " << chains[i] << " type is ";
      sBuff << elt->type().name() << " expecting " << type->name();
      GUM_ERROR(WrongType, sBuff.str());
    }
  }
}

Idx
PRMFactory::__checkAggType_exists_count(Class* c, const std::vector<std::string>& chains,
                                                  const std::vector<std::string>& params)
{
  Type* type = 0; // The type on which the aggregator applies
  for (size_t i = 0; i < chains.size(); ++i) {
    ClassElement* elt = 0;
    try {
      elt = &(c->get(chains[i]));
    } catch (NotFound&) {
      // a new slot chain?
      c->add(__buildSlotChain(c, chains[i]));
      elt = &(c->get(chains[i]));
    }
    if (type == 0) {
      type = &(elt->type());
    } else if ( (*type) != elt->type() ) {
      std::stringstream sBuff;
      sBuff << "slot chain " << chains[i] << " type is ";
      sBuff << elt->type().name() << " expecting " << type->name();
      GUM_ERROR(WrongType, sBuff.str());
    }
  }
  Idx label = 0;
  if (params.size() == 1) {
    label = type->variable().domainSize() + 1;
    for (Idx i = 0; i < type->variable().domainSize(); ++i) {
      if (type->variable().label(i) == params[0]) {
        label = i;
        break;
      }
    }
    if (label > type->variable().domainSize()) {
      GUM_ERROR(OperationNotAllowed, "Invalide parameter");
    }
    // The parameter is the label on which the aggregator applies.
    return label;
  } else {
    std::stringstream sBuff;
    sBuff << " aggregator requiring at lest one parameter";
    GUM_ERROR(OperationNotAllowed, sBuff.str());
  }
}

void
PRMFactory::addArray(const std::string& type,
                     const std::string& name, Size size)
{
  Model* model = __checkStackModel(1);
  Class* c = __retrieveClass(type);
  Instance* inst = 0;
  try {
    model->addArray(name, *c);
    for (Size i = 0; i < size; ++i) {
      std::stringstream elt_name;
      elt_name << name << "[" << i << "]";
      inst = new Instance(elt_name.str(), *c);
      //model->add(inst);
      model->add(name, inst);
      inst = 0;
    }
  } catch (OperationNotAllowed& e) {
    if (inst != 0) {
      delete inst;
    }
    throw e;
  }
}

void
PRMFactory::__setReferenceSlotWithArray(const std::string& left_instance,
                                        const std::string& left_reference,
                                        const std::string& right_instance)
{
  Model* model = __checkStackModel(1);
  Instance& left = model->get(left_instance);
  const Sequence<Instance*>& right = model->getArray(right_instance);

  ClassElement& elt = left.get(left_reference);
  if (elt.elt_type() == ClassElement::prm_refslot) {
    for (Sequence<Instance*>::iterator iter = right.begin(); iter != right.end(); ++iter) {
      __setReferenceSlotWithInstance(left_instance, left_reference, (*iter)->name());
    }
  } else {
    GUM_ERROR(OperationNotAllowed, left_instance + " is not a ReferenceSlot");
  }
}

void
PRMFactory::__addAggregatorNoParam(const std::string& name,
                                   const std::string& agg_type,
                                   const std::vector<std::string>& chains)
{
  Class* c = __checkStackClass(1);
  Type* type = 0; // The type on which the aggregator applies
  Sequence<ClassElement*> chains_seq;
  for (size_t i = 0; i < chains.size(); ++i) {
    ClassElement* elt = 0;
    try {
      elt = &(c->get(chains[i]));
    } catch (NotFound&) {
      // a new slot chain?
      c->add(__buildSlotChain(c, chains[i]));
      elt = &(c->get(chains[i]));
    }
    if (type == 0) {
      type = &(elt->type());
      chains_seq.insert(elt);
    } else if ( (*type) == elt->type() ) {
      chains_seq.insert(elt);
    } else {
      std::stringstream sBuff;
      sBuff << "slot chain " << chains[i] << " type is ";
      sBuff << elt->type().name() << " expecting " << type->name();
      GUM_ERROR(WrongType, sBuff.str());
    }
  }
  Aggregate* agg = 0;
  switch (Aggregate::str2enum(agg_type)) {
    case Aggregate::agg_min:
    case Aggregate::agg_max:
      agg = new Aggregate(name, Aggregate::str2enum(agg_type), *type);
      break;
    case Aggregate::agg_or:
    case Aggregate::agg_and:
      agg = new Aggregate(name, Aggregate::str2enum(agg_type), *__retrieveType("boolean"));
      break;
    default:
      GUM_ERROR(WrongType, agg_type);
  }
  try {
    c->add(agg);
    for (Sequence<ClassElement*>::iterator iter = chains_seq.begin(); iter != chains_seq.end(); ++iter) {
      c->insertArc((*iter)->name(), name);
    }
  } catch (DuplicateElement& e) {
    delete agg;
    throw e;
  }
}

void
PRMFactory::__addAggregatorOneParam(const std::string& name,
                                    const std::string& agg_type,
                                    const std::vector<std::string>& chains,
                                    Idx label)
{
  Class* c = __checkStackClass(1);
  Type* type = 0; // The type on which the aggregator applies
  Sequence<ClassElement*> chains_seq;
  for (size_t i = 0; i < chains.size(); ++i) {
    ClassElement* elt = 0;
    try {
      elt = &(c->get(chains[i]));
    } catch (NotFound&) {
      // a new slot chain?
      c->add(__buildSlotChain(c, chains[i]));
      elt = &(c->get(chains[i]));
    }
    if (type == 0) {
      type = &(elt->type());
      chains_seq.insert(elt);
    } else if ( (*type) == elt->type() ) {
      chains_seq.insert(elt);
    } else {
      std::stringstream sBuff;
      sBuff << "slot chain " << chains[i] << " type is ";
      sBuff << elt->type().name() << " expecting " << type->name();
      GUM_ERROR(WrongType, sBuff.str());
    }
  }
  Aggregate* agg = 0;
  switch(Aggregate::str2enum(agg_type)) {
    case Aggregate::agg_exists:
    case Aggregate::agg_forall:
      agg = new Aggregate(name, Aggregate::str2enum(agg_type), *__retrieveType("boolean"), label);
      break;
    default:
      GUM_ERROR(WrongType, agg_type);
  }
  try {
    c->add(agg);
    for (Sequence<ClassElement*>::iterator iter = chains_seq.begin(); iter != chains_seq.end(); ++iter) {
      c->insertArc((*iter)->name(), name);
    }
  } catch (DuplicateElement& e) {
    delete agg;
    throw e;
  }
}

SlotChain*
PRMFactory::__buildSlotChain(Class* start, const std::string& name)
{
  std::vector<std::string> v;
  decomposePath(name, v);
  Class* previous = start;
  ClassElement* last = 0;
  ReferenceSlot* ref = 0;
  Sequence<ReferenceSlot*> elts;
  for (size_t i = 0; i < v.size(); ++i) {
    try {
      switch (previous->get(v[i]).elt_type()) {
        case ClassElement::prm_refslot:
          ref = &(static_cast<ReferenceSlot&>(previous->get(v[i])));
          elts.insert(ref);
          previous = &(const_cast<Class &>(ref->slotType()));
          break;
        case ClassElement::prm_aggregate:
        case ClassElement::prm_attribute:
          if (i == v.size() - 1) {
            last = &(previous->get(v[i]));
            break;
          }
        default:
          std::string msg = " in ";
          GUM_ERROR(WrongType, v[i] + msg + name);
      }
    } catch (NotFound&) {
      std::string msg = " in ";
      GUM_ERROR(NotFound, v[i] + msg + name);
    }
  }
  GUM_ASSERT(v.size() == (elts.size() + 1));
  return new SlotChain(name, *start, *previous, elts, *last);
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// = ==========================================================================
