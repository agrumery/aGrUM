/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {


    INLINE PRMType& PRMType::superType() {
      if (_superType_) {
        return *_superType_;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.")
      }
    }


    INLINE DiscreteVariable& PRMType::variable() { return *_var_; }


    INLINE const DiscreteVariable& PRMType::variable() const { return *_var_; }


    INLINE DiscreteVariable& PRMType::operator*() { return *_var_; }


    INLINE const DiscreteVariable& PRMType::operator*() const { return *_var_; }


    INLINE DiscreteVariable* PRMType::operator->() { return _var_; }


    INLINE DiscreteVariable const* PRMType::operator->() const { return _var_; }


    INLINE PRMObject::prm_type PRMType::obj_type() const { return PRMObject::prm_type::TYPE; }


    INLINE const std::string& PRMType::name() const { return PRMObject::name(); }


    INLINE const PRMType& PRMType::superType() const {
      if (_superType_) {
        return *_superType_;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.")
      }
    }


    INLINE void PRMType::setSuper(PRMType& t) {
      try {
        if (t != superType()) {
          GUM_ERROR(TypeError, "The given type is not a valid super type for this Type")
        }
      } catch (NotFound const&) { GUM_ERROR(OperationNotAllowed, "This Type has no super Type") }

      _superType_ = &t;
    }


    INLINE const std::vector< Idx >& PRMType::label_map() const {
      if (_label_map_) {
        return *_label_map_;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.")
      }
    }


    INLINE bool PRMType::isSubType() const { return _superType_ != nullptr; }


    INLINE bool PRMType::isSuperTypeOf(const PRMType& t) const { return t.isSubTypeOf(*this); }


    INLINE bool PRMType::operator==(const PRMObject& obj) const { return name() == obj.name(); }

    INLINE bool PRMType::operator==(const PRMType& obj) const { return name() == obj.name(); }


    INLINE bool PRMType::operator!=(const PRMObject& obj) const { return !operator==(obj); }

    INLINE bool PRMType::operator!=(const PRMType& obj) const { return !operator==(obj); }

  } /* namespace prm */
} /* namespace gum */
