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
/**
 * @file
 * @brief Source implementation of gum::Interface.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/interface.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/interface.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {
// ============================================================================

  Interface::Interface(const std::string& name):
    ClassElementContainer(name)
  {
    GUM_CONSTRUCTOR( Interface );
  }

  Interface::Interface(const std::string& name, Interface& mother):
    ClassElementContainer(name, mother, false)
  {
    GUM_CONSTRUCTOR( Interface );
  }

  Interface::Interface(const Interface& source):
    ClassElementContainer(source)
  {
    GUM_CONS_CPY( Interface );
    GUM_ERROR(FatalError, "Illegal call to copy constructor.");
  }

  Interface::~Interface() {
    GUM_DESTRUCTOR( Interface );
  }

  PRMObject::ObjectType
  Interface::obj_type() const {
    return PRMObject::prm_interface;
  }

  Interface&
  Interface::super() { return static_cast<Interface&>(_alternate()); }

  const Interface&
  Interface::super() const { return static_cast<const Interface&>(_alternate()); }

  void
  Interface::add(Attribute* attr) {
    try {
      _add(attr);
    } catch (DuplicateElement& e) {
      if (not _exists(get(attr->name()).id())) {
        _overload(attr, get(attr->name()));
      } else {
        throw e;
      }
    }
  }

  void
  Interface::add(Aggregate* agg) { _add(agg); }

  void
  Interface::add(ReferenceSlot* ref) {
    try {
      _add(ref);
    } catch (DuplicateElement& e) {
      if (not _exists(get(ref->name()).id())) {
        _overload(ref, get(ref->name()));
      } else {
        throw e;
      }
    }
  }

  Interface&
  Interface::operator=(const Interface& source) {
    GUM_ERROR(FatalError, "Illegal call to copy operator.");
  }

  bool
  Interface::_isSubTypeOf(const ClassElementContainer& cec) const {
    GUM_ERROR(FatalError, "Not implemented!");
  }

// ============================================================================
} /* namespace prm */
} // namespace gum
// ============================================================================
