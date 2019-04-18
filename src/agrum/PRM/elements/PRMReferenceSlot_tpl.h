
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
 * @brief Inline implementation of gum::PRMReferenceSlot
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/PRMReferenceSlot.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >::PRMReferenceSlot(
       const std::string&                      name,
       PRMClassElementContainer< GUM_SCALAR >& type,
       bool                                    isArray) :
        PRMClassElement< GUM_SCALAR >(name),
        __slotType(type), __isArray(isArray) {
      GUM_CONSTRUCTOR(PRMReferenceSlot);
      this->_safeName =
         PRMObject::LEFT_CAST() + type.name() + PRMObject::RIGHT_CAST() + name;
    }

    // Destructor.
    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >::~PRMReferenceSlot() {
      GUM_DESTRUCTOR(PRMReferenceSlot);
    }

    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >::PRMReferenceSlot(
       const PRMReferenceSlot< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source),
        __slotType(source.__slotType), __isArray(source.__isArray) {
      GUM_CONS_CPY(PRMReferenceSlot);
      GUM_ERROR(FatalError,
                "illegal call to gum::ReferenceSlot copy constructor.");
    }

    // Copy operator. Raise a FatalError.
    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >& PRMReferenceSlot< GUM_SCALAR >::
                                    operator=(const PRMReferenceSlot< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy operator.");
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMClassElement< GUM_SCALAR >::ClassElementType
       PRMReferenceSlot< GUM_SCALAR >::elt_type() const {
      return this->prm_refslot;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >&
           PRMReferenceSlot< GUM_SCALAR >::slotType() {
      return __slotType;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElementContainer< GUM_SCALAR >&
                 PRMReferenceSlot< GUM_SCALAR >::slotType() const {
      return __slotType;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMReferenceSlot< GUM_SCALAR >::isArray() const {
      return __isArray;
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMReferenceSlot< GUM_SCALAR >::type() {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRMReferenceSlot< GUM_SCALAR >::type() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template < typename GUM_SCALAR >
    INLINE Potential< GUM_SCALAR >& PRMReferenceSlot< GUM_SCALAR >::cpf() {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template < typename GUM_SCALAR >
    INLINE const Potential< GUM_SCALAR >&
                 PRMReferenceSlot< GUM_SCALAR >::cpf() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >*
           PRMReferenceSlot< GUM_SCALAR >::getCastDescendant() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }


    template < typename GUM_SCALAR >
    INLINE void PRMReferenceSlot< GUM_SCALAR >::addParent(
       const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    INLINE void PRMReferenceSlot< GUM_SCALAR >::addChild(
       const PRMClassElement< GUM_SCALAR >& elt) {}

  } /* namespace prm */
} /* namespace gum */
