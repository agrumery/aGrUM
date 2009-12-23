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
 * @brief Inline implementation of gum::ClassElementContainer
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

INLINE
const DAG&
ClassElementContainer::dag() const { return *__dag; }

INLINE
const ArcSet&
ClassElementContainer::children(NodeId node) const { return dag().children(node); }

INLINE
const ArcSet&
ClassElementContainer::children(const ClassElement& elt) const { return dag().children(elt.id()); }

INLINE
const ArcSet&
ClassElementContainer::parents(NodeId node) const { return dag().parents(node); }

INLINE
const ArcSet&
ClassElementContainer::parents(const ClassElement& elt) const { return dag().parents(elt.id()); }

INLINE
bool
ClassElementContainer::exists(NodeId id) const
{
  try {
    __get(id);
    return true;
  } catch (NotFound&) {
    return false;
  }
}

INLINE
bool
ClassElementContainer::exists(const std::string& name) const
{
  try {
    __get(name);
    return true;
  } catch (NotFound&) {
    return false;
  }
}

INLINE
ClassElement&
ClassElementContainer::get(NodeId id)
{
  return *__get(id);
}

INLINE
const ClassElement&
ClassElementContainer::get(NodeId id) const
{
  return *__get(id);
}

INLINE
ClassElement&
ClassElementContainer::get(const std::string& name)
{
  return *__get(name);
}

INLINE
const ClassElement&
ClassElementContainer::get(const std::string& name) const
{
  return *__get(name);
}

INLINE
ClassElement&
ClassElementContainer::operator[](NodeId id)
{
  return *(__get(id));
}

INLINE
const ClassElement&
ClassElementContainer::operator[](NodeId id) const
{
  return *(__get(id));
}

INLINE
ClassElement&
ClassElementContainer::operator[](const std::string& name)
{
  return *(__get(name));
}

INLINE
const ClassElement&
ClassElementContainer::operator[](const std::string& name) const
{
  return *(__get(name));
}

INLINE
bool
ClassElementContainer::_exists(NodeId id) const { return __nodeIdMap.exists(id); }

INLINE
void
ClassElementContainer::_add(Attribute* attr)
{
  __add(attr);
  __attributes.insert(attr);
}

INLINE
void
ClassElementContainer::_add(Attribute* attr, NodeId id, bool overload)
{
  try {
    switch (_alternate().get(id).elt_type()) {
      case ClassElement::prm_attribute: {
                                          __attributes.erase(static_cast<Attribute*>(&(_alternate().get(id))));
                                          break;
                                        }
      case ClassElement::prm_aggregate: {
                                          __aggregates.erase(static_cast<Aggregate*>(&(_alternate().get(id))));
                                          break;
                                        }
      default: {
                 GUM_ERROR(OperationNotAllowed, "An Attribute can only replace an Attribute or an Aggregate.");
               }
    }
  } catch (NotFound&) {
    // No alternate
  }
  if (overload) {
    __dag->eraseParents(id);
  }
  attr->setId(id);
  __nameMap.insert(attr->name(), attr);
  __nodeIdMap.insert(id, attr);
  __attributes.insert(attr);
}

INLINE
void
ClassElementContainer::_add(Aggregate* agg, NodeId id, bool overload)
{
  try {
    switch (_alternate().get(id).elt_type()) {
      case ClassElement::prm_attribute: {
                                          __attributes.erase(static_cast<Attribute*>(&(_alternate().get(id))));
                                          break;
                                        }
      case ClassElement::prm_aggregate: {
                                          __aggregates.erase(static_cast<Aggregate*>(&(_alternate().get(id))));
                                          break;
                                        }
      default: {
                 GUM_ERROR(OperationNotAllowed, "An Aggregate can only replace an Attribute or an Aggregate.");
               }
    }
  } catch (NotFound&) {
    // No alternate
  }
  if (overload) {
    __dag->eraseParents(id);
  }
  agg->setId(id);
  __nameMap.insert(agg->name(), agg);
  __nodeIdMap.insert(id, agg);
  __aggregates.insert(agg);
}

INLINE
void
ClassElementContainer::_add(Aggregate* agg)
{
  __add(agg);
  __aggregates.insert(agg);
}

INLINE
void
ClassElementContainer::_add(ReferenceSlot* ref)
{
  __add(ref);
  __referenceSlots.insert(ref);
}

INLINE
void
ClassElementContainer::_add(ReferenceSlot* ref, NodeId id)
{
  try {
    switch (_alternate().get(id).elt_type()) {
      case ClassElement::prm_refslot: {
                                        __referenceSlots.erase(static_cast<ReferenceSlot*>(&(_alternate().get(id))));
                                        break;
                                      }
      default: {
                 GUM_ERROR(OperationNotAllowed, "Can replace a ReferenceSlot with only a ReferenceSlot.");
               }
    }
  } catch (NotFound&) {
    // No alternate
  }
  ref->setId(id);
  __nameMap.insert(ref->name(), ref);
  __nodeIdMap.insert(id, ref);
  __referenceSlots.insert(ref);
}

INLINE
void
ClassElementContainer::_add(SlotChain* sc)
{
  __add(sc);
  __slotChains.insert(sc);
}

INLINE
void
ClassElementContainer::_insertArc(const std::string& tail, const std::string& head)
{
  ClassElement* parent = __get(tail);
  ClassElement* child  = __get(head);
  if ( (parent->elt_type() == ClassElement::prm_refslot) or
       (child->elt_type() == ClassElement::prm_refslot) or
       (child->elt_type() == ClassElement::prm_slotchain) )
  {
    std::stringstream msg;
    msg << ": illegal arc insertion (" << tail << " -> " << head << ") in " << this->name();
    GUM_ERROR(OperationNotAllowed, msg.str());
  }
  __dag->insertArc(parent->id(), child->id());
  parent->addChild(*child);
  child->addParent(*parent);
}

INLINE
ClassElementContainer&
ClassElementContainer::_alternate()
{
  if (__alternate != 0) {
    return *__alternate;
  } else {
    std::stringstream msg;
    msg << ": no alternate class in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}


INLINE
const ClassElementContainer&
ClassElementContainer::_alternate() const
{
  if (__alternate != 0) {
    return *__alternate;
  } else {
    std::stringstream msg;
    msg << ": no alternate class in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}
INLINE
void
ClassElementContainer::__add(ClassElement* elt) {
  if (__delegateDAG) {
    std::stringstream msg;
    msg << ": can not add gum::ClassElement " << elt->name() << " in " << this->name();
    GUM_ERROR(OperationNotAllowed, msg.str());
  }
  __nameMap.insert(elt->name(), elt);
  elt->setId(__dag->insertNode());
  __nodeIdMap.insert(elt->id(), elt);
}

INLINE
ClassElement*
ClassElementContainer::__get(const std::string& name) {
  try {
    return __nameMap[name];
  } catch (NotFound&) {
    if (__alternate != 0)
      return __alternate->__get(name);
    else {
      std::stringstream msg;
      msg << ": gum::ClassElement with name " << name << " in " << this->name();
      GUM_ERROR(NotFound, msg.str());
    }
  }
}

INLINE
const ClassElement*
ClassElementContainer::__get(const std::string& name) const {
  try {
    return __nameMap[name];
  } catch (NotFound& e) {
    if (__alternate != 0)
      return __alternate->__get(name);
    else {
      std::stringstream msg;
      msg << ": gum::ClassElement with name " << name << " in " << this->name();
      GUM_ERROR(NotFound, msg.str());
    }
  }
}

INLINE
ClassElement*
ClassElementContainer::__get(NodeId id)
{
  try {
    return __nodeIdMap[id];
  } catch (NotFound& e) {
    if (__alternate != 0)
      return __alternate->__get(id);
    else {
      std::stringstream msg;
      msg << ": gum::ClassElement with id " << id << " in " << this->name();
      GUM_ERROR(NotFound, msg.str());
    }
  }
}

INLINE
const ClassElement*
ClassElementContainer::__get(NodeId id) const
{
  try {
    return __nodeIdMap[id];
  } catch (NotFound& e) {
    if (__alternate != 0)
      return __alternate->__get(id);
    else {
      std::stringstream msg;
      msg << ": gum::ClassElement with id " << id << " in " << this->name();
      GUM_ERROR(NotFound, msg.str());
    }
  }
}

INLINE
Set< Attribute* >&
ClassElementContainer::attributes() {
  return __attributes;
}

INLINE
const Set< Attribute* >&
ClassElementContainer::attributes() const {
  return __attributes;
}

INLINE
Set< Aggregate* >&
ClassElementContainer::aggregates() {
  return __aggregates;
}

INLINE
const Set< Aggregate* >&
ClassElementContainer::aggregates() const {
  return __aggregates;
}

INLINE
Set< ReferenceSlot* >&
ClassElementContainer::referenceSlots() {
  return __referenceSlots;
}

INLINE
const Set< ReferenceSlot* >&
ClassElementContainer::referenceSlots() const {
  return __referenceSlots;
}

INLINE
Set< SlotChain* >&
ClassElementContainer::slotChains() {
  return __slotChains;
}

INLINE
const Set< SlotChain* >&
ClassElementContainer::slotChains() const {
  return __slotChains;
}

INLINE
void
ClassElementContainer::_overload(Attribute* attr, ClassElement& elt) {
  try {
    if (attr->type().isSubTypeOf(elt.type())) {
      _add(attr, elt.id(), true);
    } else {
      GUM_ERROR(OperationNotAllowed, "Invalid overload type.");
    }
  } catch (OperationNotAllowed&) {
    GUM_ERROR(OperationNotAllowed, "Invalid overload element.");
  }
}

INLINE
void
ClassElementContainer::_overload(ReferenceSlot* ref, ClassElement& elt) {
  if (elt.elt_type() == ClassElement::prm_refslot) {
    ReferenceSlot& source = static_cast<ReferenceSlot&>(elt);
    ClassElementContainer* ref_type = (ClassElementContainer*) &(ref->slotType());
    if (ref_type->isSubTypeOf((const ClassElementContainer&) source.slotType())) {
      _add(ref, source.id());
    } else {
      GUM_ERROR(OperationNotAllowed, "Invalid overload slot type.");
    }
  } else {
    GUM_ERROR(OperationNotAllowed, "Invalid overload element.");
  }
}

INLINE
void
ClassElementContainer::_overload(Aggregate* agg, ClassElement& elt) {
  try {
    if (agg->type().isSubTypeOf(elt.type())) {
      _add(agg, elt.id(), true);
    } else {
      GUM_ERROR(OperationNotAllowed, "Invalid overload type.");
    }
  } catch (OperationNotAllowed&) {
    GUM_ERROR(OperationNotAllowed, "Invalid overload element.");
  }
}

INLINE
bool
ClassElementContainer::isSubTypeOf(const ClassElementContainer& cec) const {
  return ((*this) == cec) or _isSubTypeOf(cec);
}

INLINE
bool
ClassElementContainer::isSuperTypeOf(const ClassElementContainer& cec) const {
  return ((*this) == cec) or cec._isSubTypeOf(*this);
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
