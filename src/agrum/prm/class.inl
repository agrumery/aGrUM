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
 * @brief Inline implementation of gum::Class
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
// ============================================================================

INLINE
PRMObject::ObjectType
Class::obj_type() const { return prm_class; }

INLINE
Class&
Class::mother()
{
  try {
    return static_cast<Class&>(_alternate());;
  } catch (NotFound&) {
    std::stringstream msg;
    msg << ": no mother class in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}

INLINE
const Class&
Class::mother() const
{
  try {
    return static_cast<const Class&>(_alternate());;
  } catch (NotFound&) {
    std::stringstream msg;
    msg << ": no mother class in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}

INLINE
void
Class::add(Attribute* attr) {
  try {
    _add(attr);
  } catch (DuplicateElement& e) {
    if (not _exists(get(attr->name()).id())) {
      __overload(attr, get(attr->name()));
    } else {
      throw e;
    }
  }
  if (attr->type().isSubType()) {
    __addSuperType(attr);
  }
}

INLINE
void
Class::add(Aggregate* agg) { _add(agg); }

INLINE
void
Class::add(ReferenceSlot* ref) {
  try {
    _add(ref);
  } catch (DuplicateElement& e) {
    if (not _exists(get(ref->name()).id())) {
      __overload(ref, get(ref->name()));
    } else {
      throw e;
    }
  }
}

INLINE
void
Class::add(SlotChain* sc) { _add(sc); }

INLINE
void
Class::insertArc(const std::string& tail, const std::string& head)
{
  _insertArc(tail, head);
  if (get(tail).elt_type() == ClassElement::prm_slotchain) {
    SlotChain& sc = static_cast<SlotChain&>(get(tail));
    sc.end().setOutputNode(sc.lastElt().id(), true);
    setInputNode(get(head).id(), true);
  }
}

INLINE
bool
Class::operator>(const Class& daughter) const
{
  try {
    return (*this >= daughter.mother());
  } catch (NotFound&) {
    return false;
  }
}

INLINE
bool
Class::operator>=(const Class& daughter) const
{
  try {
    return ((*this == daughter) || (*this >= daughter.mother()));
  } catch (NotFound&) {
    return false;
  }
}

INLINE
bool
Class::operator<(const Class& mother) const
{
  return (mother > *this);
}

INLINE
bool
Class::operator<=(const Class& mother) const
{
  return (mother >= *this);
}

INLINE
const Sequence<NodeId>&
Class::instantiations() const { return __instantiations; }

INLINE
bool
Class::isInputNode(NodeId id) const {
  try {
    return __IOFlags[id]->first;
  } catch (NotFound&) {
    return false;
  }
}

INLINE
void
Class::setInputNode(NodeId id, bool b) {
  try {
    __IOFlags[id]->first = b;
  } catch (NotFound&) {
    __IOFlags.insert(id, new std::pair<bool, bool>(b, false));
  }
}

INLINE
bool
Class::isOutputNode(NodeId id) const {
  try {
    return __IOFlags[id]->second;
  } catch (NotFound&) {
    return false;
  }
}

INLINE
void
Class::setOutputNode(NodeId id, bool b) {
  try {
    __IOFlags[id]->second = b;
  } catch (NotFound&) {
    __IOFlags.insert(id, new std::pair<bool, bool>(false, b));
  }
}

INLINE
bool
Class::isInnerNode(NodeId id) const {
  return (not __IOFlags.exists(id));
}

// ============================================================================
} /* namespace gum */
// ============================================================================
