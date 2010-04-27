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
ClassElementContainer::ClassElementContainer(const std::string& name):
  PRMObject(name)
{
  GUM_CONSTRUCTOR( ClassElementContainer );
}

INLINE
ClassElementContainer::~ClassElementContainer() {
  GUM_DESTRUCTOR( ClassElementContainer );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator Iterator;
  for (Iterator iter = __IOFlags.begin(); iter != __IOFlags.end(); ++iter) {
    delete *iter;
  }
}

INLINE
ClassElementContainer&
ClassElementContainer::operator=(const ClassElementContainer& source) {
  GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy operator");
}

INLINE
ClassElementContainer::ClassElementContainer(const ClassElementContainer& source):
  PRMObject(source)
{
  GUM_CONS_CPY( ClassElementContainer );
  GUM_ERROR(FatalError, "illegal call to ClassElementContainer copy constructor");
}

INLINE
Property<std::pair<bool, bool>*>::onNodes&
ClassElementContainer::_IOFlags() { return __IOFlags; }

INLINE
const Property<std::pair<bool, bool>*>::onNodes&
ClassElementContainer::_IOFlags() const { return __IOFlags; }

INLINE
bool
ClassElementContainer::isInputNode(NodeId id) const {
  if (exists(id) and (ClassElement::isAttribute(get(id)) or
                      ClassElement::isAggregate(get(id))) )
  {
    try {
      return __IOFlags[id]->first;
    } catch (NotFound&) {
      return false;
    }
  } else if (not exists(id)) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  } else {
    GUM_ERROR(WrongClassElement, "given id is not an Attribute or an Aggregate");
  }
}

INLINE
void
ClassElementContainer::setInputNode(NodeId id, bool b) {
  if (exists(id) and (ClassElement::isAttribute(get(id)) or
                      ClassElement::isAggregate(get(id))) )
  {
    try {
      __IOFlags[id]->first = b;
    } catch (NotFound&) {
      __IOFlags.insert(id, new std::pair<bool, bool>(b, false));
    }
  } else if (not exists(id)) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  } else {
    GUM_ERROR(WrongClassElement, "given id is not an Attribute or an Aggregate");
  }
}

INLINE
bool
ClassElementContainer::isOutputNode(NodeId id) const {
  if (exists(id) and (ClassElement::isAttribute(get(id)) or
                      ClassElement::isAggregate(get(id))) )
  {
    try {
      return __IOFlags[id]->second;
    } catch (NotFound&) {
      return false;
    }
  } else if (not exists(id)) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  } else {
    GUM_ERROR(WrongClassElement, "given id is not an Attribute or an Aggregate");
  }
}

INLINE
void
ClassElementContainer::setOutputNode(NodeId id, bool b) {
  if (exists(id) and (ClassElement::isAttribute(get(id)) or
                      ClassElement::isAggregate(get(id))) )
  {
    try {
      __IOFlags[id]->second = b;
    } catch (NotFound&) {
      __IOFlags.insert(id, new std::pair<bool, bool>(false, b));
    }
  } else if (not exists(id)) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  } else {
    GUM_ERROR(WrongClassElement, "given id is not an Attribute or an Aggregate");
  }
}

INLINE
bool
ClassElementContainer::isInnerNode(NodeId id) const {
  if (exists(id) and (ClassElement::isAttribute(get(id)) or
                      ClassElement::isAggregate(get(id))) )
  {
    return (__IOFlags.exists(id))?not(__IOFlags[id]->first or __IOFlags[id]->second)
                                 :true;
  } else if (not exists(id)) {
    GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
  } else {
    GUM_ERROR(WrongClassElement, "given id is not an Attribute or an Aggregate");
  }
}

INLINE
bool
ClassElementContainer::isSuperTypeOf(const ClassElementContainer& cec) const {
  return cec.isSubTypeOf(*this);
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
