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
// ============================================================================

INLINE
PRMObject::ObjectType
Instance::obj_type() const { return prm_instance; }

INLINE
Class&
Instance::type()
{
  try {
    return static_cast<Class&>(_alternate());
  } catch (NotFound&) {
    std::stringstream msg;
    msg << ": no type for instance " << this->name();
    GUM_ERROR(FatalError, msg.str());
  }
}

INLINE
const Class&
Instance::type() const
{
  try {
    return static_cast<const Class&>(_alternate());
  } catch (NotFound&) {
    std::stringstream msg;
    msg << ": no type for instance " << this->name();
    GUM_ERROR(FatalError, msg.str());
  }
}

INLINE
bool
Instance::isInstantiated(NodeId id) const { return _exists(id); }

INLINE
const Set<Instance*>&
Instance::getInstances(NodeId id) const
{
  try {
    return *(__referenceMap[id]);
  } catch (NotFound& e) {
    std::stringstream msg;
    msg << ": instances set for node " <<  id << " in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}

INLINE
const Instance&
Instance::getInstance(NodeId id) const {
  try {
    if (getInstances(id).size() == 1) {
      return **(getInstances(id).begin());
    } else {
      std::stringstream msg;
      msg << get(id).name() << " is multiple";
      GUM_ERROR(OperationNotAllowed, msg.str());
    }
  } catch (NotFound& e) {
    std::stringstream msg;
    msg << ": instance for node " <<  id << " in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}

INLINE
Instance&
Instance::getInstance(NodeId id) {
  try {
    if (getInstances(id).size() == 1) {
      return **(getInstances(id).begin());
    } else {
      std::stringstream msg;
      msg << get(id).name() << " is multiple";
      GUM_ERROR(OperationNotAllowed, msg.str());
    }
  } catch (NotFound& e) {
    std::stringstream msg;
    msg << ": instance for node " <<  id << " in " << this->name();
    GUM_ERROR(NotFound, msg.str());
  }
}

INLINE
const Set< Instance::InverseSC* >&
Instance::inverseSC(NodeId id) const {
  try {
    return *(__inverseSC[id]);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "the given node has no inverse slot chain.");
  }
}

INLINE
Instance::PSISCIterator
Instance::beginPSISC() const {
  return __inverseSC.begin();
}

INLINE
const Instance::PSISCIterator&
Instance::endPSISC() const {
  return __inverseSC.end();
}

INLINE
const Bijection<const DiscreteVariable*, const DiscreteVariable*>&
Instance::bijection() const {
  if (__bijection == 0) {
    __init_bijection();
  }
  return *__bijection;
}

INLINE
bool
Instance::_isSubTypeOf(const ClassElementContainer& cec) const {
  return _alternate().isSubTypeOf(cec);
}

// ============================================================================
} /* namespace gum */
// ============================================================================
