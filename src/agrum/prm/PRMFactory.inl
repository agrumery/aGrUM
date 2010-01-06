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
 * @brief Inline implementation of PRMFactory.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
PRM*
PRMFactory::prm() { return __prm; }

INLINE
PRMObject::ObjectType
PRMFactory::current() const
{
  if (__stack.size() == 0) {
    GUM_ERROR(NotFound, "No object being built.");
  }
  return __stack.back()->obj_type();
}

INLINE
void
PRMFactory::closeCurrent()
{
  if (__stack.size() > 0) {
    __stack.pop_back();
  }
}

INLINE
const std::string&
PRMFactory::currentPackage() const { return __prefix; }

INLINE
void
PRMFactory::setPackage(const std::string& name) {
  __prefix = name;
}

INLINE
void
PRMFactory::addType(const DiscreteVariable& var)
{
  std::string name = __addPrefix(var.name());
  __checkDuplicateName(name);
  DiscreteVariable* temp = var.copyFactory();
  temp->setName(name);
  Type* type = new Type(*temp);
  __prm->__typeMap.insert(name, type);
  __prm->__types.insert(type);
  delete temp;
}

INLINE
void
PRMFactory::endClass()
{
  __checkStackClass(1)->buildInstantiationSequence();
  Class* c = __checkStackClass(1);
  c->buildInstantiationSequence();
  if (not c->isValid()) {
    for (Set<Class*>::iterator iter = c->implements().begin();
         iter != c->implements().end(); ++iter) {
      if (not c->isValid(**iter)) {
        GUM_ERROR(OperationNotAllowed, "interface wrongly implemented");
      }
    }
  }
  __stack.pop_back();
}

INLINE
void
PRMFactory::startInterface(const std::string& name, const std::string& extends)
{
  std::string real_name = __addPrefix(name);
  __checkDuplicateName(real_name);
  Class* i = 0;
  Class* super = 0;
  if (extends != "") {
    try {
      super = __prm->__interfaceMap[extends];
    } catch (NotFound&) {
      try {
        super = __prm->__interfaceMap[extends];
      } catch (NotFound&) {
        __throwNotDeclared(PRMObject::prm_interface, extends);
      }
    }
  }
  if (super != 0) {
    i = new Class(real_name, *super);
  } else {
    i = new Class(real_name);
  }
  __prm->__interfaceMap.insert(i->name(), i);
  __prm->__interfaces.insert(i);
  __stack.push_back(i);
}

INLINE
void
PRMFactory::endInterface() {
  __checkStackInterface(1);
  __stack.pop_back();
}

INLINE
void
PRMFactory::startAttribute(const std::string& type, const std::string& name)
{
  Class* c = __checkStackClassOrInterface(1);
  Attribute* a = new Attribute(name, *__retrieveType(type));
  c->add(a);
  __stack.push_back(a);
}

INLINE
void
PRMFactory::addParent(const std::string& name)
{
  // Retrieving pointers
  Attribute* a = __checkStackAttr(1);
  Class*     c = __checkStackClass(2);
  try {
    ClassElement& elt = c->get(name);
    switch(elt.elt_type()) {
      case ClassElement::prm_refslot:
        GUM_ERROR(OperationNotAllowed, "Cannot add a gum::ReferenceSlot as a parent of a gum::Attribute.");
        break;
      case ClassElement::prm_slotchain:
        if (static_cast<SlotChain&>(elt).isMultiple()) {
          GUM_ERROR(OperationNotAllowed, "Impossible to add a multiple reference slot as"
                                         " direct parent of an Attribute.");
        }
      case ClassElement::prm_attribute:
      case ClassElement::prm_aggregate:
        c->insertArc(name, a->name());
        break;
      default:
        GUM_ERROR(FatalError, "Unknown gum::ClassElement type.");
    }
  } catch (NotFound&) {
    // Check if name is a slot chain
    SlotChain* sc = __buildSlotChain(c, name);
    if (not sc->isMultiple()) {
      c->add(sc);
      c->insertArc(sc->name(), a->name());
    } else {
      delete sc;
      GUM_ERROR(OperationNotAllowed, "Impossible to add a multiple reference slot as"
                                     " direct parent of an Attribute.");
    }
  }
}

INLINE
void
PRMFactory::endAttribute()
{
  __checkStackAttr(1);
  __stack.pop_back();
}

INLINE
void
PRMFactory::addReferenceSlot(const std::string& type,
                             const std::string& name,
                             bool isArray)
{
  // Checking possible names
  std::string type_full_name;
  if (__prm->__classMap.exists(type))
    type_full_name = type;
  else if (__prm->__classMap.exists(__addPrefix(type)))
    type_full_name = __addPrefix(type);
  else
    __throwNotDeclared(PRMObject::prm_class, type);
  // Now we can proceed
  Class* owner = __checkStackClassOrInterface(1);
  Class* slotType = 0;
  try {
    slotType = __prm->__classMap[type_full_name];
  } catch (NotFound&) {
    slotType = __prm->__interfaceMap[type_full_name];
  }
  ReferenceSlot* ref = new ReferenceSlot(name, *slotType, isArray);
  try {
    owner->add(ref);
  } catch (DuplicateElement& e) {
    delete ref;
    throw e;
  }
}

INLINE
void
PRMFactory::addAggregator(const std::string& name,
                          const std::string& agg_type,
                          const std::vector<std::string>& chains,
                          const std::vector<std::string>& params)
{
  Class* c = __checkStackClass(1);
  // Different treatments for different types of aggregator.
  switch (Aggregate::str2enum(agg_type)) {
    case Aggregate::agg_or:
    case Aggregate::agg_and: {
                               __checkAggType_or_and(c, chains, params);
                               // No break
                             }
    case Aggregate::agg_min:
    case Aggregate::agg_max: {
                               __addAggregatorNoParam(name, agg_type, chains);
                               break;
                             }
    case Aggregate::agg_exists:
    case Aggregate::agg_count: {
                                 Idx label = __checkAggType_exists_count(c, chains, params);
                                 __addAggregatorOneParam(name, agg_type, chains, label);
                                 break;
                               }
    default: {
               GUM_ERROR(FatalError, "Unknown aggregator.");
             }
  }
}

INLINE
std::string
PRMFactory::__addPrefix(const std::string& str) const
{
  if (! __prefix.empty()) {
    std::string full_name = __prefix;
    full_name.append(".");
    full_name.append(str);
    return full_name;
  } else {
    return str;
  }
}

INLINE
void
PRMFactory::__checkDuplicateName(const std::string& name)
{
  if (__prm->__classMap.exists(name)     ||
      __prm->__typeMap.exists(name)      ||
      __prm->__interfaceMap.exists(name) ||
      __prm->__modelMap.exists(name))
  {
    std::stringstream msg;
    msg << "\"" << name << "\" is already used.";
    GUM_ERROR(DuplicateElement, msg.str());
  }
}

INLINE
void
PRMFactory::__throwNotDeclared(PRMObject::ObjectType obj_type,
                               const std::string& name)
{
  std::stringstream msg;
  msg << PRMObject::enum2str(obj_type) << " not declared: ";
  msg << name;
  GUM_ERROR(OperationNotAllowed, msg.str());
}

INLINE
void
PRMFactory::__throwNotDeclared(ClassElement::ClassElementType obj_type,
                               const std::string& name)
{
  std::stringstream msg;
  msg << ClassElement::enum2str(obj_type) << " not declared: ";
  msg << name;
  GUM_ERROR(OperationNotAllowed, msg.str());
}

INLINE
void
PRMFactory::__throwWrongType(PRMObject::ObjectType wrong_type,
                             const std::string& name,
                             PRMObject::ObjectType in)
{
  std::stringstream msg;
  msg << "Wrong element in slot chain " << name << " which is of type ";
  msg << PRMObject::enum2str(wrong_type) << " in " << PRMObject::enum2str(in);
  GUM_ERROR(OperationNotAllowed, msg.str());
}

INLINE
void
PRMFactory::__throwWrongType(ClassElement::ClassElementType wrong_type,
                             const std::string& name,
                             ClassElement::ClassElementType in)
{
  std::stringstream msg;
  msg << "Wrong element in slot chain " << name << " which is of type ";
  msg << ClassElement::enum2str(wrong_type) << " in " << ClassElement::enum2str(in);
  GUM_ERROR(OperationNotAllowed, msg.str());
}


INLINE
PRMObject*
PRMFactory::__checkStack(Idx i, PRMObject::ObjectType obj_type)
{
  // Don't forget that Idx are unsigned int
  if (__stack.size() - i > __stack.size()) {
    GUM_ERROR(FactoryInvalidState, "Stack not consistent with the "
                                   "current declaration.");
  }
  PRMObject* obj = __stack[__stack.size() - i];
  if (obj->obj_type() != obj_type) {
    GUM_ERROR(FactoryInvalidState, "Stack not consistent with the "
                                   "current declaration.");
  }
  return obj;
}

INLINE
Type*
PRMFactory::__checkStackType(Idx i) {
  return static_cast<Type*>(__checkStack(i, PRMObject::prm_type));
}

INLINE
Class*
PRMFactory::__checkStackClassOrInterface(Idx i)
{
  return static_cast<Class*>(__checkStack(i, PRMObject::prm_class));
}

INLINE
Class*
PRMFactory::__checkStackClass(Idx i)
{
  Class* c = static_cast<Class*>(__checkStack(i, PRMObject::prm_class));
  if (__prm->__classMap.exists(c->name())) {
    return c;
  } else {
    GUM_ERROR(FactoryInvalidState, "Not a class.");
  }
}

INLINE
Class*
PRMFactory::__checkStackInterface(Idx i)
{
  Class* c = static_cast<Class*>(__checkStack(i, PRMObject::prm_class));
  if (__prm->__interfaceMap.exists(c->name())) {
    return c;
  } else {
    GUM_ERROR(FactoryInvalidState, "Not an interface.");
  }
}

INLINE
ReferenceSlot*
PRMFactory::__checkStackRefSlot(Idx i)
{
  ClassElement* elt = static_cast<ClassElement*>(__checkStack(i, PRMObject::prm_class_elt));
  if (elt->elt_type() == ClassElement::prm_refslot) {
    return static_cast<ReferenceSlot*>(elt);
  } else {
    GUM_ERROR(FactoryInvalidState, "Stack not consistent with the "
                                   "current declaration.");
  }
}

INLINE
Attribute*
PRMFactory::__checkStackAttr(Idx i)
{
  ClassElement* elt = static_cast<ClassElement*>(__checkStack(i, PRMObject::prm_class_elt));
  if (elt->elt_type() == ClassElement::prm_attribute) {
    return static_cast<Attribute*>(elt);
  } else {
    GUM_ERROR(FactoryInvalidState, "Stack not consistent with the "
                                   "current declaration.");
  }
}

INLINE
Aggregate*
PRMFactory::__checkStackAgg(Idx i)
{
  ClassElement* elt = static_cast<ClassElement*>(__checkStack(i, PRMObject::prm_class_elt));
  if (elt->elt_type() == ClassElement::prm_aggregate) {
    return static_cast<Aggregate*>(elt);
  } else {
    GUM_ERROR(FactoryInvalidState, "Stack not consistent with the "
                                   "current declaration.");
  }
}

INLINE
Model*
PRMFactory::__checkStackModel(Idx i)
{
  return static_cast<Model*>(__checkStack(i, PRMObject::prm_model));
}

INLINE
SlotChain*
PRMFactory::__checkStackSC(Idx i)
{
  ClassElement* elt = static_cast<ClassElement*>(__checkStack(i, PRMObject::prm_class_elt));
  SlotChain* sc = dynamic_cast< SlotChain* >(elt);
  if (elt != 0) {
    return sc;
  } else {
    GUM_ERROR(FactoryInvalidState, "Stack not consistent with the "
                                   "current declaration.");
  }
}

INLINE
Type*
PRMFactory::__retrieveType(const std::string& name)
{
  try {
    return __prm->__typeMap[name];
  } catch (NotFound&) {
    return  __prm->__typeMap[__addPrefix(name)];
  }
  return 0;
}

INLINE
Class*
PRMFactory::__retrieveClass(const std::string& name)
{
  try {
    return __prm->__classMap[name];
  } catch (NotFound&) {
    try {
      return __prm->__classMap[__addPrefix(name)];
    } catch (NotFound&) {
      __throwNotDeclared(PRMObject::prm_class, name);
    }
  }
  // Just for compilation warnings
  return 0;
}

INLINE
void
PRMFactory::startModel(const std::string& name)
{
  __checkDuplicateName(__addPrefix(name));
  // Telling all classes to build their "nodes to be instantiated" set
  for (Sequence<Class*>::iterator iter = __prm->__classes.begin(); iter != __prm->__classes.end(); ++iter) {
    (*iter)->buildInstantiationSequence();
  }
  Model* model = new Model(__addPrefix(name));
  __stack.push_back(model);
}

INLINE
void
PRMFactory::endModel()
{
  Model* model = __checkStackModel(1);
  __stack.pop_back();
  model->instantiate();
  __prm->__modelMap.insert(model->name(), model);
  __prm->__models.insert(model);
}

INLINE
void
PRMFactory::addInstance(const std::string& type, const std::string& name)
{
  Model* model = __checkStackModel(1);
  Class* c = __retrieveClass(type);
  Instance* inst = new Instance(name, *c);
  try {
    model->add(inst);
  } catch (OperationNotAllowed& e) {
    delete inst;
    throw e;
  }
}

INLINE
void
PRMFactory::setReferenceSlot(const std::string& left_instance,
                             const std::string& left_reference,
                             const std::string& right_instance)
{
  Model* model = __checkStackModel(1);
  if (model->isInstance(right_instance)) {
    __setReferenceSlotWithInstance(left_instance, left_reference, right_instance);
  } else if (model->isArray(right_instance)) {
    __setReferenceSlotWithArray(left_instance, left_reference, right_instance);
  } else {
    GUM_ERROR(NotFound, right_instance);
  }
}

INLINE
void
PRMFactory::__setReferenceSlotWithInstance(const std::string& left_instance,
                                           const std::string& left_reference,
                                           const std::string& right_instance)
{
  Model* model = __checkStackModel(1);
  Instance& left = model->get(left_instance);
  Instance& right = model->get(right_instance);

  ClassElement& elt = left.get(left_reference);
  if (elt.elt_type() == ClassElement::prm_refslot) {
    left.add(elt.id(), right);
  } else {
    GUM_ERROR(OperationNotAllowed, left_instance + " is not a ReferenceSlot");
  }
}

INLINE
void
PRMFactory::startDiscreteType(const std::string& name) {
  std::string real_name = __addPrefix(name);
  __checkDuplicateName(real_name);
  Type* t = new Type(LabelizedVariable(real_name, "", 0));
  __stack.push_back(t);
}

INLINE
void
PRMFactory::addLabel(const std::string& l) {
  Type* t = __checkStackType(1);
  LabelizedVariable* var = dynamic_cast<LabelizedVariable*>(t->__var);
  if (not var) {
    GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.");
  } else if (t->__super) {
    GUM_ERROR(OperationNotAllowed, "current type is a subtype.");
  }
  var->addLabel(l);
}

INLINE
void
PRMFactory::startDiscreteType(const std::string& name,
                              const std::string& super) {
  std::string real_name = __addPrefix(name);
  __checkDuplicateName(real_name);
  Type* t = new Type(LabelizedVariable(real_name, "", 0));
  t->__super = __retrieveType(super);
  t->__label_map = new std::vector<Idx>();
  __stack.push_back(t);
}

INLINE
void
PRMFactory::addLabel(const std::string& l, std::string& extends) {
  Type* t = __checkStackType(1);
  LabelizedVariable* var = dynamic_cast<LabelizedVariable*>(t->__var);
  if (not var) {
    GUM_ERROR(FatalError, "the current type's variable is not a LabelizedVariable.");
  } else if (not t->__super) {
    GUM_ERROR(OperationNotAllowed, "current type is not a subtype.");
  }
  bool found = false;
  for (Idx i = 0; i < t->__super->__var->domainSize(); ++i) {
    if (t->__super->__var->label(i) == extends) {
      var->addLabel(l);
      t->__label_map->push_back(i);
      found = true;
      break;
    }
  }
  if (not found) {
    GUM_ERROR(OperationNotAllowed, "inexistent label in super type.");
  }
}

INLINE
void
PRMFactory::endDiscreteType() {
  Type* t = __checkStackType(1);
  if (not t->__isValid()) {
    GUM_ERROR(OperationNotAllowed, "current type is not a valid subtype.");
  }
  __prm->__typeMap.insert(t->name(), t);
  __prm->__types.insert(t);
  __stack.pop_back();
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
