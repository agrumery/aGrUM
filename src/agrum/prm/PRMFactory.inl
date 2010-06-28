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
PRMFactory::PRMFactory()
{
  GUM_CONSTRUCTOR( PRMFactory );
  __prm = new PRM();
}

INLINE
PRMFactory::~PRMFactory()
{
  GUM_DESTRUCTOR( PRMFactory );
}

INLINE
PRM*
PRMFactory::prm() { return __prm; }

INLINE
PRMObject::ObjectType
PRMFactory::currentType() const
{
  if (__stack.size() == 0) {
    GUM_ERROR(NotFound, "no object being built");
  }
  return __stack.back()->obj_type();
}

INLINE
PRMObject*
PRMFactory::getCurrent() {
  if (__stack.size() == 0) {
    GUM_ERROR(NotFound, "no object being built");
  }
  return __stack.back();
}

INLINE
PRMObject*
PRMFactory::closeCurrent()
{
  if (__stack.size() > 0) {
    PRMObject* obj = __stack.back();
    __stack.pop_back();
    return obj;
  } else {
    return 0;
  }
}

INLINE
std::string
PRMFactory::currentPackage() const { return (__packages.empty())?"":__packages.back(); }

INLINE
void
PRMFactory::startDiscreteType(const std::string& name,
                              std::string super) {
  if (super == "") {
    std::string real_name = __addPrefix(name);
    __checkDuplicateName(real_name);
    Type* t = new Type(LabelizedVariable(real_name, "", 0));
    __stack.push_back(t);
  } else {
    std::string real_name = __addPrefix(name);
    __checkDuplicateName(real_name);
    Type* t = new Type(LabelizedVariable(real_name, "", 0));
    t->__super = __retrieveType(super);
    t->__label_map = new std::vector<Idx>();
    __stack.push_back(t);
  }
}

INLINE
void
PRMFactory::endDiscreteType() {
  Type* t = static_cast<Type*>(__checkStack(1, PRMObject::prm_type));
  if (not t->__isValid()) {
    GUM_ERROR(OperationNotAllowed, "current type is not a valid subtype");
  } else if (t->variable().domainSize() < 2) {
    GUM_ERROR(OperationNotAllowed, "current tyoe is not a valid discrete type");
  }
  __prm->__typeMap.insert(t->name(), t);
  __prm->__types.insert(t);
  __stack.pop_back();
}

INLINE
void
PRMFactory::endInterface() {
  __checkStack(1, PRMObject::prm_interface);
  __stack.pop_back();
}

INLINE
void
PRMFactory::addAttribute(const std::string& type, const std::string& name) {
  __checkStack(1, PRMObject::prm_interface);
  startAttribute(type, name);
  endAttribute();
}

INLINE
void
PRMFactory::startAttribute(const std::string& type, const std::string& name)
{
  ClassElementContainer* c = __checkStackContainter(1);
  Attribute* a = new Attribute(name, *__retrieveType(type));
  std::string dot = ".";
  try {
    c->add(a);
  } catch (DuplicateElement&) {
    c->overload(a);
  }
  __stack.push_back(a);
}

INLINE
void
PRMFactory::setRawCPFByLines(const std::vector<prm_float>& array)
{
  Attribute* a = static_cast<Attribute*>(__checkStack(1, ClassElement::prm_attribute));
  __checkStack(2, PRMObject::prm_class);
  if (a->cpf().domainSize() != array.size())
    GUM_ERROR(OperationNotAllowed, "illegal CPF size");
  a->cpf().fillWith(array);
}

INLINE
void
PRMFactory::endAttribute()
{
  __checkStack(1, ClassElement::prm_attribute);
  __stack.pop_back();
}

INLINE
void
PRMFactory::startSystem(const std::string& name)
{
  __checkDuplicateName(__addPrefix(name));
  System* model = new System(__addPrefix(name));
  __stack.push_back(model);
}

INLINE
void
PRMFactory::endSystem()
{
  System* model = static_cast<System*>(__checkStack(1, PRMObject::prm_system));
  __stack.pop_back();
  model->instantiate();
  __prm->__systemMap.insert(model->name(), model);
  __prm->__systems.insert(model);
}

INLINE
void
PRMFactory::addInstance(const std::string& type, const std::string& name)
{
  System* model = static_cast<System*>(__checkStack(1, PRMObject::prm_system));
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
std::string
PRMFactory::__addPrefix(const std::string& str) const
{
  if (not __packages.empty()) {
    std::string full_name = __packages.back();
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
      __prm->__systemMap.exists(name))
  {
    std::stringstream msg;
    msg << "\"" << name << "\" is already used.";
    GUM_ERROR(DuplicateElement, msg.str());
  }
}

INLINE
PRMObject*
PRMFactory::__checkStack(Idx i, PRMObject::ObjectType obj_type)
{
  // Don't forget that Idx are unsigned int
  if (__stack.size() - i > __stack.size()) {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
  PRMObject* obj = __stack[__stack.size() - i];
  if (obj->obj_type() != obj_type) {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
  return obj;
}

INLINE
ClassElementContainer*
PRMFactory::__checkStackContainter(Idx i) {
  // Don't forget that Idx are unsigned int
  if (__stack.size() - i > __stack.size()) {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
  PRMObject* obj = __stack[__stack.size() - i];
  if ( (obj->obj_type() == PRMObject::prm_class) or
       (obj->obj_type() == PRMObject::prm_interface) ) {
    return static_cast<ClassElementContainer*>(obj);
  } else {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
}

INLINE
ClassElement*
PRMFactory::__checkStack(Idx i, ClassElement::ClassElementType elt_type)
{
  // Don't forget that Idx are unsigned int
  if (__stack.size() - i > __stack.size()) {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
  ClassElement* obj = dynamic_cast<ClassElement*>(__stack[__stack.size() - i]);
  if (obj == 0) {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
  if (obj->elt_type() != elt_type) {
    GUM_ERROR(FactoryInvalidState, "illegal sequence of calls");
  }
  return obj;
}

INLINE
Type*
PRMFactory::__retrieveType(const std::string& name) const
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
PRMFactory::__retrieveClass(const std::string& name) const
{
  try {
    return __prm->__classMap[name];
  } catch (NotFound&) {
    try {
      return __prm->__classMap[__addPrefix(name)];
    } catch (NotFound&) {
      std::string msg = "could not found Class: ";
      GUM_ERROR(NotFound, msg + name);
    }
  }
  // Just for compilation warnings
  return 0;
}

INLINE
Interface*
PRMFactory::__retrieveInterface(const std::string& name) const
{
  try {
    return __prm->__interfaceMap[name];
  } catch (NotFound&) {
    try {
      return __prm->__interfaceMap[__addPrefix(name)];
    } catch (NotFound&) {
      std::string msg = "could not found Interface: ";
      GUM_ERROR(NotFound, msg + name);
    }
  }
  // Just for compilation warnings
  return 0;
}

INLINE
int
PRMFactory::__typeDepth(Type* t) {
  int depth = 0;
  Type* current = t;
  while (current->isSubType()) {
    ++depth;
    current = &(current->super());
  }
  return depth;
}

INLINE
void
PRMFactory::pushPackage(const std::string& name) {
  __packages.push_back(name);
}

INLINE
std::string
PRMFactory::popPackage() {
  if (not __packages.empty()) {
    std::string s = __packages.back();
    __packages.pop_back();
    return s;
  }
  return "";
}

INLINE
void
PRMFactory::setReferenceSlot(const std::string& l_i,
                             const std::string& r_i)
{
  size_t pos = l_i.find_last_of('.');
  if (pos != std::string::npos) {
    std::string l_ref = l_i.substr(pos + 1, std::string::npos);
    setReferenceSlot(l_i.substr(0, pos), l_ref, r_i);
  } else {
    GUM_ERROR(NotFound, "left value does not name an instance or an array");
  }
}

INLINE
Class&
PRMFactory::retrieveClass(const std::string& name) {
  return *__retrieveClass(name);
}

INLINE
Type&
PRMFactory::retrieveType(const std::string& name) {
  return *__retrieveType(name);
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
