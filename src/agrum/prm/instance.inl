/**************************************************************************
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
 * @brief Inline implementation of gum::Instance
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

INLINE
Size
Instance::size() const {
  return __nodeIdMap.size();
}

INLINE
void
Instance::__copyAggregates(Aggregate* source) {
  Attribute* attr = new Attribute(source->name(), source->type(), source->buildImpl());
  GUM_ASSERT(&(attr->type().variable()) != &(source->type().variable()));
  attr->setId(source->id());
  __nodeIdMap.insert(attr->id(), attr);
  __trash.insert(attr);
  __bijection.insert(&(attr->type().variable()), &(source->type().variable()));
}

INLINE
void
Instance::__copyAttribute(Attribute* source) {
  Attribute* attr = new Attribute(source->name(), source->type());
  // The potential is copied when instantiation is called
  attr->cpf().fill((prm_float) 0);
  attr->setId(source->id());
  __trash.insert(attr);
  __bijection.insert(&(attr->type().variable()), &(source->type().variable()));
  __nodeIdMap.insert(attr->id(), attr);
}

INLINE
Instance::Instance(const Instance& source):
  PRMObject(source), __type(source.__type), __params(0)
{
  GUM_CONS_CPY( Instance );
  GUM_ERROR(FatalError, "do not copy Instance");
}

INLINE
Instance&
Instance::operator=(const Class& from) {
  GUM_ERROR(FatalError, "do not copy Instance");
}

INLINE
PRMObject::ObjectType
Instance::obj_type() const { return PRMObject::prm_instance; }

INLINE
Class&
Instance::type() { return *__type; }

INLINE
const Class&
Instance::type() const { return *__type; }

INLINE
bool
Instance::exists(NodeId id) const {
  return __nodeIdMap.exists(id);
}

INLINE
Attribute&
Instance::get(NodeId id) {
  try {
    return *(__nodeIdMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no Attribute with the given NodeId");
  }
}

INLINE
const Attribute&
Instance::get(NodeId id) const {
  try {
    return *(__nodeIdMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no Attribute with the given NodeId");
  }
}

INLINE
Attribute&
Instance::get(const std::string& name) {
  try {
    return *(__nodeIdMap[type().get(name).id()]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no Attribute with the given name");
  }
}

INLINE
const Attribute&
Instance::get(const std::string& name) const {
  try {
    return *(__nodeIdMap[type().get(name).id()]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no Attribute with the given name");
  }
}

INLINE
bool
Instance::isInitialised(NodeId id) const {
  return (__params)?__params->exists(id):false;
}

INLINE
void
Instance::setParameterValue(const std::string& name, const Potential<prm_float>& value) {
  Attribute* attr = 0;
  try {
    attr = &(get(name));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "the given ClassElement is not in this Instance");
  }
  if (not __type->isParameter(*attr)) {
    GUM_ERROR(WrongClassElement, "the given NodeId is not a parameter");
  }
  if (attr->cpf().nbrDim() > 1) {
    GUM_ERROR(FatalError, "found a parameter's potential with more than one dimension");
  }
  if (not value.contains(attr->type().variable())) {
    GUM_ERROR(OperationNotAllowed, "the given value is invalid for this parameter");
  }
  // Assigning value
  attr->cpf().copy(value);
  if (not __params) {
    __params = new Set<NodeId>();
  }
  __params->insert(attr->id());
}

INLINE
bool
Instance::isInstantiated(NodeId id) const {
  try {
    return &(get(id).type().variable()) != &(type().get(id).type().variable());
  } catch (NotFound&) {
    return __referenceMap.exists(id);
  }
}

INLINE
void
Instance::__addReferingInstance(SlotChain* sc, Instance* i) {
  NodeId id = i->get(sc->lastElt().safeName()).id();
  std::string name = sc->lastElt().safeName();
  try {
    i->__referenceMap[id]->insert(this);
    i->__referingAttr[id]->push_back(std::make_pair(this, sc->name()));
  } catch (NotFound&) {
    i->__referenceMap.insert(id, new Set< Instance* >());
    i->__referenceMap[id]->insert(this);
    i->__referingAttr.insert(id, new std::vector<pair>());
    i->__referingAttr[id]->push_back(std::make_pair(this, sc->name()));
  }
}

INLINE
void
Instance::instantiate(NodeId id)
{
  if (not type().exists(id)) {
    GUM_ERROR(NotFound, "no ClassElement matches the given id");
  } else if (__nodeIdMap.exists(id)) {
    GUM_ERROR(WrongClassElement, "can not instantiate the given ClassElement");
  }
  if (not __instantiated_nodes.exists(id)) {
    __copyAttribute(__nodeIdMap[id]);
    // Could be optimised
    instantiate();
  }
}

INLINE
const Bijection<const DiscreteVariable*, const DiscreteVariable*>&
Instance::bijection() const {
  return __bijection;
}

INLINE
const Instance&
Instance::getInstance(NodeId id) const {
  try {
    if (__referenceMap[id]->size() > 0) {
      return **(__referenceMap[id]->begin());
    } else {
      GUM_ERROR(UndefinedElement, "no Instance associated with the given NodeId");
    }
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ReferenceSlot or SlotChain matches the given NodeId");
  }
}

INLINE
const Set<Instance*>&
Instance::getInstances(NodeId id) const {
  try {
    return *(__referenceMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ReferenceSlot or SlotChain matches the given NodeId");
  }
}

INLINE
Instance::iterator
Instance::begin() { return __nodeIdMap.begin(); }

INLINE
const Instance::iterator&
Instance::end() { return __nodeIdMap.end(); }

INLINE
Instance::const_iterator
Instance::begin() const { return __nodeIdMap.begin(); }

INLINE
const Instance::const_iterator&
Instance::end() const { return __nodeIdMap.end(); }

INLINE
Instance::RefIterator
Instance::begin(NodeId id) {
  try {
    return Instance::RefIterator(*(__referenceMap[id]));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no referred instances from this NodeId");
  }
}

INLINE
Instance::RefConstIterator
Instance::begin(NodeId id) const {
  try {
    return Instance::RefConstIterator(*(__referenceMap[id]));
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no referred instances from this NodeId");
  }
}

INLINE
Instance::RefIterator::RefIterator(Set<Instance*>& set):
  __set(set), __iter(set.begin())
{
  GUM_CONSTRUCTOR( Instance::RefIterator );
}

INLINE
Instance::RefIterator::RefIterator( const RefIterator& from ):
  __set(const_cast< Set<Instance*>& >(from.__set)), __iter(from.__iter)
{
  GUM_CONS_CPY( Instance::RefIterator );
}

INLINE
Instance::RefIterator::~RefIterator() {
  GUM_DESTRUCTOR( Instance::RefIterator );
}

INLINE
Instance::RefIterator&
Instance::RefIterator::operator=( const RefIterator& from ) {
  __iter = from.__iter;
  return *this;
}

INLINE
Instance::RefIterator&
Instance::RefIterator::operator++() {
  ++__iter;
  return *this;
}

INLINE
bool
Instance::RefIterator::isEnd() const {
  return __iter == __set.end();
}

INLINE
bool
Instance::RefIterator::operator!=(const RefIterator& from) const {
  return __iter != from.__iter;
}

INLINE
bool
Instance::RefIterator::operator==(const RefIterator& from) const {
  return __iter == from.__iter;
}

INLINE
Instance&
Instance::RefIterator::operator*() const {
  return **__iter;
}

INLINE
Instance*
Instance::RefIterator::operator->() const {
  return *__iter;
}

INLINE
Instance::RefConstIterator::RefConstIterator(const Set<Instance*>& set):
  __set(set), __iter(set.begin())
{
  GUM_CONSTRUCTOR( Instance::RefConstIterator );
}

INLINE
Instance::RefConstIterator::RefConstIterator( const RefConstIterator& from ):
  __set(from.__set), __iter(from.__iter)
{
  GUM_CONS_CPY( Instance::RefConstIterator );
}

INLINE
Instance::RefConstIterator::~RefConstIterator() {
  GUM_DESTRUCTOR( Instance::RefConstIterator );
}

INLINE
Instance::RefConstIterator&
Instance::RefConstIterator::operator=( const RefConstIterator& from ) {
  __iter = from.__iter;
  return *this;
}

INLINE
Instance::RefConstIterator&
Instance::RefConstIterator::operator++() {
  ++__iter;
  return *this;
}

INLINE
bool
Instance::RefConstIterator::isEnd() const {
  return __iter == __set.end();
}

INLINE
bool
Instance::RefConstIterator::operator!=(const RefConstIterator& from) const {
  return __iter != from.__iter;
}

INLINE
bool
Instance::RefConstIterator::operator==(const RefConstIterator& from) const {
  return __iter == from.__iter;
}

INLINE
const Instance&
Instance::RefConstIterator::operator*() const {
  return **__iter;
}

INLINE
const Instance*
Instance::RefConstIterator::operator->() const {
  return *__iter;
}

INLINE
Instance::InvRefIterator
Instance::beginInvRef() {
  return __referingAttr.begin();
}

INLINE
const Instance::InvRefIterator&
Instance::endInvRef() {
  return __referingAttr.end();
}

INLINE
Instance::InvRefConstIterator
Instance::beginInvRef() const {
  return __referingAttr.begin();
}

INLINE
const Instance::InvRefConstIterator&
Instance::endInvRef() const {
  return __referingAttr.end();
}

INLINE
std::vector< std::pair<Instance*, std::string> >&
Instance::getRefAttr(NodeId id) {
  return *(__referingAttr[id]);
}

INLINE
const std::vector< std::pair<Instance*, std::string> >&
Instance::getRefAttr(NodeId id) const {
  return *(__referingAttr[id]);
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
