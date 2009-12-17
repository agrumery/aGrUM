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
 * @brief Inline implementation of type.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
namespace gum {

  INLINE
  const DiscreteVariable&
  Type::variable() const { return *__var; }

  INLINE
  DiscreteVariable&
  Type::operator*() { return *__var; }

  INLINE
  const DiscreteVariable&
  Type::operator*() const { return *__var; }

  INLINE
  DiscreteVariable*
  Type::operator->() const { return __var; }

  INLINE
  bool
  Type::operator==(const Type& from) const { return (name() == from.name()); }

  INLINE
  bool
  Type::operator!=(const Type& from) const {return (name() != from.name());}

  INLINE
  PRMObject::ObjectType
  Type::obj_type() const { return prm_type; }

  INLINE
  const std::string&
  Type::name() const { return PRMObject::name(); }

  INLINE
  const Type&
  Type::super() const {
    if (not __super) {
      return *__super;
    } else {
      GUM_ERROR(NotFound, "No super type for this type.");
    }
  }

  INLINE
  const std::vector<Idx>&
  Type::label_map() const {
    if (not __label_map) {
      return *__label_map;
    } else {
      GUM_ERROR(NotFound, "No super type for this type.");
    }
  }

  INLINE
  bool
  Type::operator<(const Type& from) const {
    if (__super) {
      return (__super->name() == from.name()) or ((*__super) < from);
    } else {
      return false;
    }
  }

  INLINE
  bool
  Type::operator<=(const Type& from) const {
    if (__super) {
      return (name() == from.name()) or ((*__super) <= from);
    } else {
      return name() == from.name();
    }
  }

  INLINE
  bool
  Type::operator>(const Type& from) const {
    return from < *this;
  }

  INLINE
  bool
  Type::operator>=(const Type& from) const {
    return from <= *this;
  }

  INLINE
  bool
  Type::isSubType() const { return __super; }

} /* namespace gum */
// ============================================================================
