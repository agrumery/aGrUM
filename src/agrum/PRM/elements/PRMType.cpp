
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Source implementation of type.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN (@LIP6)
 *
 */

#include <agrum/PRM/elements/PRMType.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/elements/PRMType_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  namespace prm {


    void PRMType::__updateName() {
      const void*       address = static_cast< const void* >(this);
      std::stringstream ss;
      ss << this->name() << "-" << address;
      this->__var->setName(ss.str());
    }


    PRMType::PRMType(const DiscreteVariable& var) :
        PRMObject(var.name()), __var(var.clone()), __superType(0), __label_map(0) {
      GUM_CONSTRUCTOR(PRMType);
      this->__updateName();
    }


    PRMType::PRMType(PRMType&                  super_type,
                     const std::vector< Idx >& label_map,
                     const DiscreteVariable&   var) :
        PRMObject(var.name()),
        __var(var.clone()), __superType(&super_type),
        __label_map(new std::vector< Idx >(label_map)) {
      GUM_CONSTRUCTOR(PRMType);
      this->__updateName();

      if (!__isValid()) {
        delete __label_map;
        __label_map = 0;
        GUM_ERROR(OperationNotAllowed, "Invalid label map.");
      }
    }


    PRMType::PRMType(const PRMType& from) :
        PRMObject(from), __var(from.__var->clone()), __superType(from.__superType),
        __label_map(0) {
      GUM_CONS_CPY(PRMType);
      this->__updateName();

      if (__superType) { __label_map = new std::vector< Idx >(from.label_map()); }
    }


    PRMType::PRMType(PRMType&& from) : PRMObject(from) {
      GUM_CONS_MOV(PRMType);
      GUM_ERROR(FatalError, "Move constructor must not be used");
    }


    PRMType::~PRMType() {
      GUM_DESTRUCTOR(PRMType);
      delete __var;
      if (__label_map) { delete __label_map; }
    }


    PRMType& PRMType::operator=(const PRMType& from) {
      GUM_ERROR(FatalError, "Copy operator must not be used");
    }


    PRMType& PRMType::operator=(PRMType&& from) {
      GUM_ERROR(FatalError, "Move operator must not be used");
    }


    bool PRMType::isSubTypeOf(const PRMType& super) const {
      if ((*this) == super) {
        return true;
      } else if (__superType) {
        return __superType->isSubTypeOf(super);
      } else {
        return false;
      }
    }


    bool PRMType::__isValid() const {
      if (!__superType) { return __var->domainSize() > 1; }

      if (__label_map->size() == __var->domainSize()) {
        for (size_t i = 0; i < __label_map->size(); ++i) {
          if (__label_map->at(i) >= (**__superType).domainSize()) { return false; }
        }

        return true;
      }

      return false;
    }


  } /* namespace prm */
} /* namespace gum */
