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

void
PRMFactory::addLabel(const std::string& l, std::string extends) {
  if (extends == "") {
    Type* t = static_cast<Type*>(__checkStack(1, PRMObject::prm_type));
    LabelizedVariable* var = dynamic_cast<LabelizedVariable*>(t->__var);
    if (not var) {
      GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.");
    } else if (t->__super) {
      GUM_ERROR(OperationNotAllowed, "current type is a subtype.");
    }
    try {
      var->addLabel(l);
    } catch (DuplicateElement&) {
      GUM_ERROR(DuplicateElement, "a label with the same value already exists");
    }
  } else {
    Type* t = static_cast<Type*>(__checkStack(1, PRMObject::prm_type));
    LabelizedVariable* var = dynamic_cast<LabelizedVariable*>(t->__var);
    if (not var) {
      GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.");
    } else if (not t->__super) {
      GUM_ERROR(OperationNotAllowed, "current type is not a subtype.");
    }
    bool found = false;
    for (Idx i = 0; i < t->__super->__var->domainSize(); ++i) {
      if (t->__super->__var->label(i) == extends) {
        try {
          var->addLabel(l);
        } catch (DuplicateElement&) {
          GUM_ERROR(DuplicateElement, "a label with the same value already exists");
        }
        t->__label_map->push_back(i);
        found = true;
        break;
      }
    }
    if (not found) {
      GUM_ERROR(NotFound, "inexistent label in super type.");
    }
  }
}

void
PRMFactory::startClass(const std::string& name, const std::string& extends,
                       const Set<std::string>* implements)
{
  std::string real_name = __addPrefix(name);
  __checkDuplicateName(real_name);
  Class* c = 0;
  Class* mother = 0;
  Set<Interface*> impl;
  if (implements != 0) {
    for (Set<std::string>::iterator iter = implements->begin();
         iter != implements->end(); ++iter) {
      try {
        impl.insert(__prm->__interfaceMap[*iter]);
      } catch (NotFound&) {
        try {
          impl.insert(__prm->__interfaceMap[__addPrefix(*iter)]);
        } catch (NotFound&) {
          std::string msg = "no interface named: ";
          GUM_ERROR(NotFound, msg + __addPrefix(*iter));
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
        std::string msg = "no class named: ";
        GUM_ERROR(NotFound, msg + extends);
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
PRMFactory::endClass()
{
  Class* c = static_cast<Class*>(__checkStack(1, PRMObject::prm_class));
  Interface* i = 0;
  std::string name;
  std::stringstream msg;
  msg << "class " << c->name() << " does not respect interface ";
  try {
    for (Set<Interface*>::iterator iter = c->implements().begin(); iter != c->implements().end(); ++iter) {
      i = *iter;
      try {
        for (DAG::NodeIterator node = i->dag().beginNodes(); node != i->dag().endNodes(); ++node) {
          name = i->get(*node).name();
          switch (i->get(*node).elt_type()) {
            case ClassElement::prm_aggregate:
            case ClassElement::prm_attribute:
              {
                if ( (c->get(name).elt_type() == ClassElement::prm_attribute) or
                     (c->get(name).elt_type() == ClassElement::prm_aggregate) )
                {
                  if (not c->get(name).type().isSubTypeOf(i->get(name).type())) {
                    GUM_ERROR(TypeError, msg.str() + i->name());
                  }
                } else {
                  GUM_ERROR(TypeError, msg.str() + i->name());
                }
                break;
              }
            case ClassElement::prm_refslot:
              {
                if (c->get(name).elt_type() == ClassElement::prm_refslot) {
                  const ReferenceSlot& ref_i = static_cast<const ReferenceSlot&>(i->get(name));
                  const ReferenceSlot& ref_this = static_cast<const ReferenceSlot&>(c->get(name));
                  if (not ref_this.slotType().isSubTypeOf(ref_i.slotType())) {
                    GUM_ERROR(TypeError, msg.str() + i->name());
                  }
                } else {
                  GUM_ERROR(TypeError, msg.str() + i->name());
                }
                break;
              }
            case ClassElement::prm_slotchain:
              {
                // Nothing to check: they are automatically inherited
                break;
              }
            default:
              {
                std::string msg = "unexpected ClassElement in interface ";
                GUM_ERROR(FatalError, msg + i->name());
              }
          }
        }
      } catch (NotFound&) {
        GUM_ERROR(TypeError, msg.str() + i->name());
      }
    }
  } catch (NotFound&) {
    // this Class does not implement any Interface
  }
  __stack.pop_back();
}

void
PRMFactory::startInterface(const std::string& name, const std::string& extends)
{
  std::string real_name = __addPrefix(name);
  __checkDuplicateName(real_name);
  Interface* i = 0;
  Interface* super = 0;
  if (extends != "") {
    try {
      super = __prm->__interfaceMap[extends];
    } catch (NotFound&) {
      try {
        super = __prm->__interfaceMap[__addPrefix(extends)];
      } catch (NotFound&) {
        std::string msg = "unknown interface: ";
        GUM_ERROR(NotFound, msg + extends);
      }
    }
  }
  if (super != 0) {
    i = new Interface(real_name, *super);
  } else {
    i = new Interface(real_name);
  }
  __prm->__interfaceMap.insert(i->name(), i);
  __prm->__interfaces.insert(i);
  __stack.push_back(i);
}

void
PRMFactory::addAttribute(Attribute* attr) {
  Class* c = static_cast<Class*>(__checkStack(1, PRMObject::prm_class));
  c->add(attr);
  Size count = 0;
  const Sequence<const DiscreteVariable*>& vars = attr->cpf().variablesSequence();
  for (DAG::NodeIterator node = c->dag().beginNodes(); node != c->dag().endNodes(); ++node) {
    try {
      if (vars.exists(&(c->get(*node).type().variable()))) {
        ++count;
        if (&(attr->type().variable()) != &(c->get(*node).type().variable())) {
          c->insertArc(c->get(*node).safeName(), attr->safeName());
        }
      }
    } catch (OperationNotAllowed&) { }
  }
  if (count != attr->cpf().variablesSequence().size()) {
    GUM_ERROR(NotFound, "unable to found all parents of this attribute");
  }
}

void
PRMFactory::addParent(const std::string& name)
{
  // Retrieving pointers
  Attribute* a = static_cast<Attribute*>(__checkStack(1, ClassElement::prm_attribute));
  ClassElementContainer* c = __checkStackContainter(2);
  try {
    ClassElement& elt = c->get(name);
    switch(elt.elt_type()) {
      case ClassElement::prm_refslot:
        {
          GUM_ERROR(OperationNotAllowed, "can not add a reference slot as a parent of an attribute");
          break;
        }
      case ClassElement::prm_slotchain:
        {
          if (static_cast<SlotChain&>(elt).isMultiple()) {
            GUM_ERROR(OperationNotAllowed, "can not add a multiple slot chain to an attribute");
          }
          c->insertArc(name, a->name());
          break;
        }
      case ClassElement::prm_attribute:
      case ClassElement::prm_aggregate:
        {
          c->insertArc(name, a->name());
          break;
        }
      default:
        {
          GUM_ERROR(FatalError, "unknown ClassElement");
        }
    }
  } catch (NotFound&) {
    // Check if name is a slot chain
    SlotChain* sc = __buildSlotChain(c, name);
    if (sc == 0) {
      std::string msg = "found no ClassElement with the given name ";
      GUM_ERROR(NotFound, msg + name);
    } else if (not sc->isMultiple()) {
      c->add(sc);
      c->insertArc(sc->name(), a->name());
    } else {
      delete sc;
      GUM_ERROR(OperationNotAllowed, "Impossible to add a multiple reference slot as"
                                     " direct parent of an Attribute.");
    }
  }
}

void
PRMFactory::setRawCPFByColumns(const std::vector<prm_float>& array)
{
  Attribute* a = static_cast<Attribute*>(__checkStack(1, ClassElement::prm_attribute));
  if (a->cpf().domainSize() != array.size()) {
    GUM_ERROR(OperationNotAllowed, "illegal CPF size");
  }
  if (a->cpf().nbrDim() == 1) {
    setRawCPFByLines(array);
  } else {
    std::vector<Size> pos(a->cpf().nbrDim(), 0);
    Instantiation inst(a->cpf());
    inst.setFirst();
    for (std::vector<prm_float>::const_iterator value = array.begin(); value != array.end(); ++value) {
      a->cpf().set(inst, *value);
      for (size_t idx = pos.size(); idx > 0; --idx) {
        bool stop = true;
        ++(pos[idx-1]);
        if (pos[idx-1] == a->cpf().variablesSequence().atPos(idx-1)->domainSize()) {
          pos[idx-1] = 0;
          stop = false;
        }
        inst.chgVal(a->cpf().variablesSequence().atPos(idx-1), pos[idx-1]);
        if (stop) {
          break;
        }
      }
    }
  }
}

void
PRMFactory::setCPFByRule(const std::vector<std::string>& parents,
                         const std::vector<prm_float>& values)
{
  Attribute* a = static_cast<Attribute*>(__checkStack(1, ClassElement::prm_attribute));
  if ((parents.size() + 1) != a->cpf().variablesSequence().size()) {
    GUM_ERROR(OperationNotAllowed, "wrong number of parents");
  }
  if (values.size() != a->type().variable().domainSize()) {
    GUM_ERROR(OperationNotAllowed, "wrong number of values");
  }
  Instantiation inst(a->cpf());
  // jnst holds parents with a specific value (not "*")
  // knst holds parents without a specific value ("*")
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
      if (not found) {
        std::string msg = "could not find label ";
        GUM_ERROR(NotFound, msg + parents[i]);
      }
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
PRMFactory::addParameter(const std::string& type, const std::string& name,
                         std::string value)
{
  Class* c = static_cast<Class*>(__checkStack(1, PRMObject::prm_class));
  if (value == "") {
    MultiDimSparse<prm_float>* impl =
      new MultiDimSparse<prm_float>(std::numeric_limits<prm_float>::signaling_NaN());
    Attribute* a = new Attribute(name, *__retrieveType(type), impl);
    c->addParameter(a, false);
  } else {
    MultiDimSparse<prm_float>* impl = new MultiDimSparse<prm_float>(0);
    Attribute* a = new Attribute(name, *__retrieveType(type), impl);
    Instantiation inst(a->cpf());
    bool found = false;
    for (inst.setFirst(); not inst.end(); inst.inc()) {
      if (a->type()->label(inst.pos(*(a->type()))) == value) {
        a->cpf().set(inst, 1);
        found = true;
        break;
      }
    }
    if (not found) {
      GUM_ERROR(NotFound, "illegal default value.");
    }
    c->addParameter(a, true);
  }
}

void
PRMFactory::addAggregator(const std::string& name,
                          const std::string& agg_type,
                          const std::vector<std::string>& chains,
                          const std::vector<std::string>& params)
{
  Class* c = static_cast<Class*>(__checkStack(1, PRMObject::prm_class));
  // Checking call legality
  if (chains.size() == 0) {
    GUM_ERROR(OperationNotAllowed, "an Aggregate requires at least one parent");
  }
  // Retrieving the parents of the aggregate
  std::vector<ClassElement*> inputs;
  __retrieveInputs(c, chains, inputs);
  // Checking that all inputs shares the same Type (trivial if inputs.size() == 1)
  Size sc_count = 0;
  try {
    if (inputs.size() > 1) {
      for (std::vector<ClassElement*>::iterator iter = inputs.begin() + 1; iter != inputs.end(); ++iter) {
        if ((**(iter - 1)).type() != (**iter).type()) {
          GUM_ERROR(WrongType, "found different types");
        }
        if (ClassElement::isSlotChain(**iter)) { ++sc_count; }
      }
      if ( (sc_count > 0) and (sc_count != inputs.size()) ) {
        GUM_ERROR(OperationNotAllowed, "aggregators cannot have different sort of parents");
      }
    } else if (ClassElement::isSlotChain(*(inputs.front()))) {
      ++sc_count;
    }
  } catch (OperationNotAllowed&) {
    GUM_ERROR(WrongClassElement, "expected an attribute, an aggregate or a slot chain");
  }
  // Different treatments for different types of aggregate.
  Aggregate* agg = 0;
  switch (Aggregate::str2enum(agg_type)) {
    case Aggregate::agg_or:
    case Aggregate::agg_and:
      {
        if (inputs.front()->type() != *(__retrieveType("boolean"))) {
          GUM_ERROR(WrongType, "expected booleans");
        }
      }
    case Aggregate::agg_min:
    case Aggregate::agg_max:
      {
        if (params.size() != 0) {
          GUM_ERROR(OperationNotAllowed, "invalid number of paramaters");
        }
        agg = new Aggregate(name, Aggregate::str2enum(agg_type), inputs.front()->type());
        break;
      }
    case Aggregate::agg_exists:
    case Aggregate::agg_forall:
      {
        if (params.size() != 1) {
          GUM_ERROR(OperationNotAllowed, "invalid number of paramaters");
        }
        Idx label_idx = 0;
        while (label_idx < inputs.front()->type()->domainSize()) {
          if (inputs.front()->type()->label(label_idx) == params.front()) {
            break;
          }
          ++label_idx;
        }
        if (label_idx == inputs.front()->type()->domainSize()) {
          GUM_ERROR(NotFound, "could not find label");
        }
        // Creating and adding the Aggregate
        agg = new Aggregate(name, Aggregate::str2enum(agg_type), *(__retrieveType("boolean")), label_idx);
        break;
      }
    default:
      { GUM_ERROR(FatalError, "Unknown aggregator."); }
  }
  std::string safe_name = agg->safeName();
  try {
    if (sc_count > 0) {
      c->add(agg);
    } else {
      // Inner aggregators can be directly used has attributes
      Attribute* attr = new Attribute(agg->name(), agg->type(), agg->buildImpl());
      c->add(attr);
      delete agg;
    }
  } catch (DuplicateElement& e) {
    delete agg;
    throw e;
  }
  for (std::vector<ClassElement*>::iterator iter = inputs.begin(); iter != inputs.end(); ++iter) {
    c->insertArc((*iter)->safeName(), safe_name);
  }
}

void
PRMFactory::addReferenceSlot(const std::string& type,
                             const std::string& name,
                             bool isArray)
{
  ClassElementContainer* owner = __checkStackContainter(1);
  ClassElementContainer* slotType = 0;
  try {
    slotType = __retrieveClass(type);
  } catch (NotFound&) {
    try {
      slotType = __retrieveInterface(type);
    } catch (NotFound&) {
      GUM_ERROR(NotFound, "unknown ReferenceSlot slot type");
    }
  }
  ReferenceSlot* ref = new ReferenceSlot(name, *slotType, isArray);
  try {
    owner->add(ref);
  } catch (DuplicateElement& e) {
    owner->overload(ref);
  }
}

void
PRMFactory::addArray(const std::string& type,
                     const std::string& name, Size size)
{
  System* model = static_cast<System*>(__checkStack(1, PRMObject::prm_system));
  Class* c = __retrieveClass(type);
  Instance* inst = 0;
  try {
    model->addArray(name, *c);
    for (Size i = 0; i < size; ++i) {
      std::stringstream elt_name;
      elt_name << name << "[" << i << "]";
      inst = new Instance(elt_name.str(), *c);
      model->add(name, inst);
    }
  } catch (TypeError& e) {
    delete inst;
    throw e;
  } catch (NotFound& e) {
    delete inst;
    throw e;
  }
}

void
PRMFactory::incArray(const std::string& l_i, const std::string& r_i)
{
  System* model = static_cast<System*>(__checkStack(1, PRMObject::prm_system));
  if (model->isArray(l_i)) {
    if (model->isInstance(r_i)) {
      model->add(l_i, model->get(r_i));
    } else {
      GUM_ERROR(NotFound, "right value is not an instance");
    }
  } else {
    GUM_ERROR(NotFound, "left value is no an array");
  }
}

void
PRMFactory::setReferenceSlot(const std::string& l_i,
                             const std::string& l_ref,
                             const std::string& r_i)
{
  typedef Sequence<Instance*>::iterator Iter;
  typedef std::vector<Instance*>::iterator Jter;
  System* model = static_cast<System*>(__checkStack(1, PRMObject::prm_system));
  std::vector<Instance*> lefts;
  std::vector<Instance*> rights;
  if (model->isInstance(l_i)) {
    lefts.push_back(&(model->get(l_i)));
  } else if (model->isArray(l_i)) {
    for (Iter iter = model->getArray(l_i).begin(); iter != model->getArray(l_i).end(); ++iter)
      lefts.push_back(*iter);
  } else {
    GUM_ERROR(NotFound, "left value does not name an instance or an array");
  }
  if (model->isInstance(r_i)) {
    rights.push_back(&(model->get(r_i)));
  } else if (model->isArray(r_i)) {
    for (Iter iter = model->getArray(r_i).begin(); iter != model->getArray(r_i).end(); ++iter)
      rights.push_back(*iter);
  } else {
    GUM_ERROR(NotFound, "left value does not name an instance or an array");
  }
  for (Jter l = lefts.begin(); l != lefts.end(); ++l) {
    for (Jter r = rights.begin(); r != rights.end(); ++r) {
      if ((**l).type().get(l_ref).elt_type() == ClassElement::prm_refslot) {
        (**l).add((**l).type().get(l_ref).id(), **r);
      } else {
        GUM_ERROR(NotFound, "unfound reference slot");
      }
    }
  }
}

void
PRMFactory::setParameter(const std::string& instance, const std::string& param, const std::string& value) {
  System* model = static_cast<System*>(__checkStack(1, PRMObject::prm_system));
  Instance* i = 0;
  try {
    i = &(model->get(instance));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "instance not found in current system");
  }
  Attribute* a = 0;
  try {
    a = &(i->get(param));
    if (not i->type().isParameter(*a)) {
      GUM_ERROR(OperationNotAllowed, "given attribute is not a parameter");
    }
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no such parameter in the given instance");
  }
  Size label = a->type()->domainSize();
  for (Size idx = 0; idx < a->type()->domainSize(); ++idx) {
    if (value == a->type()->label(idx)) {
      label = idx;
      break;
    }
  }
  if (label == a->type()->domainSize()) {
    GUM_ERROR(NotFound, "no such label in the given parameter");
  }
  Potential<prm_float> pot;
  pot.add(a->type().variable());
  pot.fill((prm_float) 0);
  Instantiation inst(pot);
  inst.chgVal(a->type().variable(), label);
  pot.set(inst, (prm_float) 1);
  i->setParameterValue(a->safeName(), pot);
}

SlotChain*
PRMFactory::__buildSlotChain(ClassElementContainer* start, const std::string& name)
{
  std::vector<std::string> v;
  decomposePath(name, v);
  ClassElementContainer* current = start;
  ReferenceSlot* ref = 0;
  Sequence<ClassElement*> elts;
  for (size_t i = 0; i < v.size(); ++i) {
    try {
      switch (current->get(v[i]).elt_type()) {
        case ClassElement::prm_refslot:
          ref = &(static_cast<ReferenceSlot&>(current->get(v[i])));
          elts.insert(ref);
          current = &(const_cast<ClassElementContainer&>(ref->slotType()));
          break;
        case ClassElement::prm_aggregate:
        case ClassElement::prm_attribute:
          if (i == v.size() - 1) {
            elts.insert(&(current->get(v[i])));
            break;
          } else {
            return 0;
          }
        default: { return 0; }
      }
    } catch (NotFound&) {
      return 0;
    }
  }
  GUM_ASSERT(v.size() == elts.size());
  current->setOutputNode(*(elts.back()), true);
  // std::stringstream sBuff;
  // sBuff << current->name() << "." << elts.back()->name() << " : " << current->isOutputNode(elts.back()->id());
  // GUM_TRACE(sBuff.str());
  return new SlotChain(name, elts);
}

void
PRMFactory::__retrieveInputs(Class* c, const std::vector<std::string>& chains,
                             std::vector<ClassElement*>& inputs)
{
  for (size_t i = 0; i < chains.size(); ++i) {
    try {
      inputs.push_back(&(c->get(chains[i])));
    } catch (NotFound&) {
      inputs.push_back(__buildSlotChain(c, chains[i]));
      if (inputs.back()) {
        c->add(inputs.back());
      } else {
        GUM_ERROR(NotFound, "unknown slot chain");
      }
    }
  }
}

Type*
PRMFactory::__retrieveCommonType(const std::vector<ClassElement*>& elts)
{
  const Type* current = 0;
  HashTable<std::string, Size> counters;
  // Finding all types and super types
  for (std::vector<ClassElement*>::const_iterator iter = elts.begin(); iter != elts.end(); ++iter)  {
    try {
      current = &((**iter).type());
      while (current != 0) {
        // Filling counters
        if (counters.exists(current->name())) {
          ++(counters[current->name()]);
        } else {
          counters.insert(current->name(), 1);
        }
        // Loop guard
        if (current->isSubType()) {
          current = &(current->super());
        } else {
          current = 0;
        }
      }
    } catch (OperationNotAllowed&) {
       GUM_ERROR(WrongClassElement, "found a ClassElement without a type");
    }
  }
  // We need to find the most specialized (i.e. max depth) common type
  current = 0;
  int max_depth = -1;
  int current_depth = 0;
  for (HashTable<std::string, Size>::iterator iter = counters.begin(); iter != counters.end(); ++iter) {
    if ( (*iter) == elts.size() ) {
      current_depth = __typeDepth(__retrieveType(iter.key()));
      if (current_depth > max_depth) {
        max_depth = current_depth;
        current = __retrieveType(iter.key());
      }
    }
  }
  if (current) {
    return const_cast<Type*>(current);
  }
  GUM_ERROR(NotFound, "could not find a common type");
}

void
PRMFactory::addNoisyOrCompound(const std::string& name,
                       const std::vector<std::string>& chains,
                       const std::vector<float>& numbers, float leak,
                       const std::vector<std::string>& labels)
{
  if (currentType() != PRMObject::prm_class) {
    GUM_ERROR(gum::FactoryInvalidState, "invalid state to add a noisy-or");
  }
  Class* c = dynamic_cast<gum::prm::Class*>(getCurrent());
  std::vector<ClassElement*> parents;
  for (std::vector<std::string>::const_iterator iter = chains.begin(); iter != chains.end(); ++iter) {
    parents.push_back(&(c->get(*iter)));
  }
  Type* common_type = __retrieveCommonType(parents);
  for (size_t idx = 0; idx < parents.size(); ++idx) {
    if (parents[idx]->type() != (*common_type)) {
      ClassElement* parent = parents[idx];
      // Either safe_name is an non existing slot chain or an existing cast descendant
      std::string safe_name = parent->cast(*common_type);
      if (not c->exists(safe_name)) {
        if (ClassElement::isSlotChain(*parent)) {
          parents[idx] = __buildSlotChain(c, safe_name);
          c->add(parents[idx]);
        } else {
          GUM_TRACE(safe_name);
          GUM_ERROR(NotFound, "unable to find parent");
        }
      } else {
        parents[idx] = &(c->get(safe_name));
      }
    }
  }
  if (numbers.size() == 1) {
    gum::prm::Attribute* attr = new gum::prm::Attribute(name, retrieveType("boolean"),
        new gum::MultiDimNoisyORCompound<gum::prm::prm_float>(leak, numbers.front()));
    addAttribute(attr);
  } else if (numbers.size() == parents.size()) {
    gum::MultiDimNoisyORCompound<gum::prm::prm_float>* noisy = new gum::MultiDimNoisyORCompound<gum::prm::prm_float>(leak);
    gum::prm::FuncAttribute* attr = new gum::prm::FuncAttribute(name, retrieveType("boolean"), noisy);
    for (size_t idx = 0; idx < numbers.size(); ++idx) {
      noisy->causalWeight(parents[idx]->type().variable(), numbers[idx]);
    }
    addAttribute(attr);
  } else {
    GUM_ERROR(OperationNotAllowed, "invalid parameters for a noisy or");
  }
  if (not labels.empty()) {
    GUM_ERROR(OperationNotAllowed, "labels definitions not handle for noisy-or");
  }
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// = ==========================================================================
