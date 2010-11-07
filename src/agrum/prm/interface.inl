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
 * @brief Inline implementation of gum::prm::Interface
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

INLINE
Interface::ClassEltIterator
Interface::begin() {
  return __nodeIdMap.begin();
}

INLINE
const Interface::ClassEltIterator&
Interface::end() {
  return __nodeIdMap.end();
}

INLINE
Interface::const_ClassEltIterator
Interface::begin() const {
  return __nodeIdMap.begin();
}

INLINE
const Interface::const_ClassEltIterator&
Interface::end() const {
  return __nodeIdMap.end();
}

INLINE
void
Interface::insertArc(const std::string& tail, const std::string& head) {
  GUM_ERROR(OperationNotAllowed, "an Interface does not have arcs");
}

INLINE
Interface&
Interface::super() {
  if (__super)  return *__super;
  else          GUM_ERROR(NotFound, "this Interface is not a sub interface");
}

INLINE
const Interface&
Interface::super() const {
  if (__super)  return *__super;
  else          GUM_ERROR(NotFound, "this Interface is not a sub interface");
}

INLINE
void
Interface::__addImplementation(Class* c) {
  __implementations.insert(c);
}

INLINE
void
Interface::__addExtension(Interface* i) {
  __extensions.insert(i);
}

INLINE
ClassElement&
Interface::operator[](NodeId id) {
  return get(id);
}

INLINE
const ClassElement&
Interface::operator[](NodeId id) const {
  return get(id);
}

INLINE
ClassElement&
Interface::operator[](const std::string& name) {
  return get(name);
}

INLINE
const ClassElement&
Interface::operator[](const std::string& name) const {
  return get(name);
}

INLINE
PRMObject::ObjectType
Interface::obj_type() const {
  return PRMObject::prm_interface;
}

INLINE
const DAG&
Interface::_dag() const { return __dag; }

INLINE
DAG&
Interface::_dag() { return __dag; }

INLINE
ClassElement&
Interface::get(NodeId id) {
  try {
    return *(__nodeIdMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  }
}

INLINE
const ClassElement&
Interface::get(NodeId id) const {
  try {
    return *(__nodeIdMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  }
}

INLINE
ClassElement&
Interface::get(const std::string& name) {
  try {
    return *(__nameMap[name]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ClassElement with the given name");
  }
}

INLINE
const ClassElement&
Interface::get(const std::string& name) const {
  try {
    return *(__nameMap[name]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no ClassElement with the given name");
  }
}

INLINE
const Set< Attribute* >&
Interface::attributes() const {
  return __attributes;
}

INLINE
const Set< ReferenceSlot* >&
Interface::referenceSlots() const {
  return __referenceSlots;
}

INLINE
Set<Class*>&
Interface::implementations() {
  return __implementations;
}

INLINE
const Set<Class*>&
Interface::implementations() const {
  return __implementations;
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
