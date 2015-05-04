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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/PRM/elements/type.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    Type<GUM_SCALAR>::Type(const DiscreteVariable &var)
        : PRMObject(var.name()), __var(var.clone()), __super(0), __label_map(0) {
      GUM_CONSTRUCTOR(Type);
    }

    template <typename GUM_SCALAR>
    Type<GUM_SCALAR>::Type(Type<GUM_SCALAR> &super_type,
                           const std::vector<Idx> &label_map,
                           const DiscreteVariable &var)
        : PRMObject(var.name()), __var(var.clone()), __super(&super_type),
          __label_map(new std::vector<Idx>(label_map)) {
      GUM_CONSTRUCTOR(Type);

      if (__isValid()) {
        delete __label_map;
        GUM_ERROR(OperationNotAllowed, "Invalid label map.");
      }
    }

    template <typename GUM_SCALAR>
    Type<GUM_SCALAR>::Type(const Type<GUM_SCALAR> &from)
        : PRMObject(from), __var(from.__var->clone()), __super(from.__super),
          __label_map(0) {
      GUM_CONS_CPY(Type);

      if (__super) {
        __label_map = new std::vector<Idx>(from.label_map());
      }
    }

    template <typename GUM_SCALAR> Type<GUM_SCALAR>::~Type() {
      GUM_DESTRUCTOR(Type);
      delete __var;
      delete __label_map;
    }

    template <typename GUM_SCALAR>
    bool Type<GUM_SCALAR>::isSubTypeOf(const Type<GUM_SCALAR> &super) const {
      if ((*this) == super) {
        return true;
      } else if (__super) {
        return __super->isSubTypeOf(super);
      } else {
        return false;
      }
    }

    template <typename GUM_SCALAR> bool Type<GUM_SCALAR>::__isValid() const {
      if (not __super) {
        return __var->domainSize() > 1;
      }

      if (__label_map->size() == __var->domainSize()) {
        for (size_t i = 0; i < __label_map->size(); ++i) {
          if (__label_map->at(i) >= (**__super).domainSize()) {
            return false;
          }
        }

        return true;
      }

      return false;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &Type<GUM_SCALAR>::super() {
      return *__super;
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable &Type<GUM_SCALAR>::variable() {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE const DiscreteVariable &Type<GUM_SCALAR>::variable() const {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable &Type<GUM_SCALAR>::operator*() {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE const DiscreteVariable &Type<GUM_SCALAR>::operator*() const {
      return *__var;
    }

    template <typename GUM_SCALAR>
    INLINE DiscreteVariable *Type<GUM_SCALAR>::operator->() const {
      return __var;
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::PRMType Type<GUM_SCALAR>::obj_type() const {
      return PRMObject::PRMType::TYPE;
    }

    template <typename GUM_SCALAR>
    INLINE const std::string &Type<GUM_SCALAR>::name() const {
      return PRMObject::name();
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR> &Type<GUM_SCALAR>::super() const {
      if (__super) {
        return *__super;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.");
      }
    }

    template <typename GUM_SCALAR>
    INLINE void Type<GUM_SCALAR>::setSuper(Type<GUM_SCALAR> &t) {
      try {
        if (t != super()) {
          GUM_ERROR(WrongType, "the given Type is not equal to this Type super.");
        }
      } catch (NotFound &) {
        GUM_ERROR(OperationNotAllowed, "this Type has no super Type");
      }

      __super = &t;
    }

    template <typename GUM_SCALAR>
    INLINE const std::vector<Idx> &Type<GUM_SCALAR>::label_map() const {
      if (__label_map) {
        return *__label_map;
      } else {
        GUM_ERROR(NotFound, "No super type for this type.");
      }
    }

    template <typename GUM_SCALAR> INLINE bool Type<GUM_SCALAR>::isSubType() const {
      return __super;
    }

    template <typename GUM_SCALAR>
    INLINE bool Type<GUM_SCALAR>::isSuperTypeOf(const Type<GUM_SCALAR> &t) const {
      return t.isSubTypeOf(*this);
    }

    template <typename GUM_SCALAR>
    INLINE bool Type<GUM_SCALAR>::operator==(const PRMObject &obj) const {
      return name() == obj.name();
    }

    template <typename GUM_SCALAR>
    INLINE bool Type<GUM_SCALAR>::operator!=(const PRMObject &obj) const {
      return name() != obj.name();
    }

  } /* namespace prm */
} /* namespace gum */
