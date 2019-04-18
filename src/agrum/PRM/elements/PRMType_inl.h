
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Inline implementation of type.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {


    INLINE PRMType& PRMType::superType() {
      if (__superType) {
        return *__superType;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.");
      }
    }


    INLINE DiscreteVariable& PRMType::variable() { return *__var; }


    INLINE const DiscreteVariable& PRMType::variable() const { return *__var; }


    INLINE DiscreteVariable& PRMType::operator*() { return *__var; }


    INLINE const DiscreteVariable& PRMType::operator*() const { return *__var; }


    INLINE DiscreteVariable* PRMType::operator->() { return __var; }


    INLINE DiscreteVariable const* PRMType::operator->() const { return __var; }


    INLINE PRMObject::prm_type PRMType::obj_type() const {
      return PRMObject::prm_type::TYPE;
    }


    INLINE const std::string& PRMType::name() const { return PRMObject::name(); }


    INLINE const PRMType& PRMType::superType() const {
      if (__superType) {
        return *__superType;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.");
      }
    }


    INLINE void PRMType::setSuper(PRMType& t) {
      try {
        if (t != superType()) {
          GUM_ERROR(WrongType,
                    "The given type is not a valid super type for this Type");
        }
      } catch (NotFound&) {
        GUM_ERROR(OperationNotAllowed, "This Type has no super Type");
      }

      __superType = &t;
    }


    INLINE const std::vector< Idx >& PRMType::label_map() const {
      if (__label_map) {
        return *__label_map;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.");
      }
    }


    INLINE bool PRMType::isSubType() const { return __superType != nullptr; }


    INLINE bool PRMType::isSuperTypeOf(const PRMType& t) const {
      return t.isSubTypeOf(*this);
    }


    INLINE bool PRMType::operator==(const PRMObject& obj) const {
      return name() == obj.name();
    }


    INLINE bool PRMType::operator!=(const PRMObject& obj) const {
      return name() != obj.name();
    }

  } /* namespace prm */
} /* namespace gum */
