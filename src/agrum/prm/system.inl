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
 * @brief Inline implementation of System.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
NodeId
System::add(const std::string& array, Instance& i) {
  return add(array, &i);
}

INLINE
const DiGraph&
System::skeleton() const { return __skeleton; }

INLINE
Instance&
System::get(NodeId id) {
  try {
    return *(__nodeIdMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no Instance matching the given id");
  }
}

INLINE
const Instance&
System::get(NodeId id) const {
  try {
    return *(__nodeIdMap[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no Instance matching the given id");
  }
}

INLINE
PRMObject::ObjectType
System::obj_type() const { return PRMObject::prm_system; }

INLINE
Size
System::size() const { return __nodeIdMap.size(); }

INLINE
bool
System::isInstantiated(const Class& c) const {
  return __instanceMap.exists(const_cast<Class*>(&c));
}

INLINE
bool
System::isInstance(const std::string& name) const {
  return __nameMap.exists(name);
}

INLINE
bool
System::isArray(const std::string& name) const {
  return __arrayMap.exists(name);
}

INLINE
void
System::instantiate() {
  for (System::iterator iter = begin(); iter != end(); ++iter) {
    (**iter).instantiate();
  }
}

INLINE
Instance&
System::get(const std::string& name) {
  try {
    return *(__nameMap[name]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no Instance matching the given name");
  }
}

INLINE
const Instance&
System::get(const std::string& name) const {
  try {
    return *(__nameMap[name]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no Instance matching the given name");
  }
}

INLINE
const Set<Instance*>&
System::get(const Class& type) const {
  try {
    return *(__instanceMap[const_cast<Class*>(&type)]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "the given Class has no instantiation in this System");
  }
}

INLINE
const Sequence<Instance*>&
System::getArray(const std::string& name) const {
  try {
    return *(__arrayMap[name].second);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
ClassElementContainer&
System::getArrayType(const std::string& name) {
  try {
    return *(__arrayMap[name].first);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
const ClassElementContainer&
System::getArrayType(const std::string& name) const {
  try {
    return *(__arrayMap[name].first);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
NodeId
System::add(const std::string& array, Instance* i) {
  try {
    if (i->type().isSubTypeOf(*(__arrayMap[array].first))) {
      NodeId id = add(i);
      __arrayMap[array].second->insert(i);
      return id;
    } else {
      GUM_ERROR(TypeError, "the given Instance is of an incorrect Class type");
    }
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
void
System::addArray(const std::string& array, ClassElementContainer& type) {
  if (__arrayMap.exists(array)) {
    GUM_ERROR(DuplicateElement, "an array with the same is already in this System");
  }
  __arrayMap.insert(array, System::model_pair(&type, new Sequence<Instance*>()));
}

INLINE
System::iterator
System::begin() { return __nodeIdMap.begin(); }

INLINE
const System::iterator&
System::end() { return __nodeIdMap.end(); }

INLINE
System::const_iterator
System::begin() const { return __nodeIdMap.begin(); }

INLINE
const System::const_iterator&
System::end() const { return __nodeIdMap.end(); }

INLINE
System::array_iterator
System::begin(const std::string& a) {
  try {
    return __arrayMap[a].second->begin();
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
const System::array_iterator&
System::end(const std::string& a) {
  try {
    return __arrayMap[a].second->end();
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
System::const_array_iterator
System::begin(const std::string& a) const {
  try {
    return __arrayMap[a].second->begin();
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

INLINE
const System::const_array_iterator&
System::end(const std::string& a) const {
  try {
    return __arrayMap[a].second->end();
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no array matching the given name");
  }
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
