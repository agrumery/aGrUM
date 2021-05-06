/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 *
 */

#include <agrum/PRM/elements/PRMType.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/elements/PRMType_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  namespace prm {


    void PRMType::updateName__() {
      const void*       address = static_cast< const void* >(this);
      std::stringstream ss;
      ss << this->name() << "-" << address;
      this->var__->setName(ss.str());
    }


    PRMType::PRMType(const DiscreteVariable& var) :
        PRMObject(var.name()), var__(var.clone()), superType__(0), label_map__(0) {
      GUM_CONSTRUCTOR(PRMType);
      this->updateName__();
    }


    PRMType::PRMType(PRMType&                  super_type,
                     const std::vector< Idx >& label_map,
                     const DiscreteVariable&   var) :
        PRMObject(var.name()),
        var__(var.clone()), superType__(&super_type),
        label_map__(new std::vector< Idx >(label_map)) {
      GUM_CONSTRUCTOR(PRMType);
      this->updateName__();

      if (!isValid__()) {
        delete label_map__;
        label_map__ = 0;
        GUM_ERROR(OperationNotAllowed, "Invalid label map.")
      }
    }


    PRMType::PRMType(const PRMType& from) :
        PRMObject(from), var__(from.var__->clone()), superType__(from.superType__),
        label_map__(0) {
      GUM_CONS_CPY(PRMType);
      this->updateName__();

      if (superType__) { label_map__ = new std::vector< Idx >(from.label_map()); }
    }


    PRMType::PRMType(PRMType&& from) : PRMObject(from) {
      GUM_CONS_MOV(PRMType);
      GUM_ERROR(FatalError, "Move constructor must not be used")
    }


    PRMType::~PRMType() {
      GUM_DESTRUCTOR(PRMType);
      delete var__;
      if (label_map__) { delete label_map__; }
    }


    PRMType& PRMType::operator=(const PRMType& from){
       GUM_ERROR(FatalError, "Copy operator must not be used")}


    PRMType& PRMType::operator=(PRMType&& from) {
      GUM_ERROR(FatalError, "Move operator must not be used")
    }


    bool PRMType::isSubTypeOf(const PRMType& super) const {
      if ((*this) == super) {
        return true;
      } else if (superType__) {
        return superType__->isSubTypeOf(super);
      } else {
        return false;
      }
    }


    bool PRMType::isValid__() const {
      if (!superType__) { return var__->domainSize() > 1; }

      if (label_map__->size() == var__->domainSize()) {
        for (size_t i = 0; i < label_map__->size(); ++i) {
          if (label_map__->at(i) >= (**superType__).domainSize()) { return false; }
        }

        return true;
      }

      return false;
    }


  } /* namespace prm */
} /* namespace gum */
