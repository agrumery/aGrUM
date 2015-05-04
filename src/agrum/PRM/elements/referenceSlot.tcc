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
 * @brief Inline implementation of gum::ReferenceSlot
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/referenceSlot.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    ReferenceSlot<GUM_SCALAR>::ReferenceSlot(const std::string &name,
                                             ClassElementContainer<GUM_SCALAR> &type,
                                             bool isArray)
        : ClassElement<GUM_SCALAR>(name), __slotType(type), __isArray(isArray) {
      GUM_CONSTRUCTOR(ReferenceSlot);
      this->_safeName = PRMObject::LEFT_CAST() + ((PRMObject &)__slotType).name() +
                        PRMObject::RIGHT_CAST() + name;
    }

    // Destructor.
    template <typename GUM_SCALAR> ReferenceSlot<GUM_SCALAR>::~ReferenceSlot() {
      GUM_DESTRUCTOR(ReferenceSlot);
    }

    template <typename GUM_SCALAR>
    ReferenceSlot<GUM_SCALAR>::ReferenceSlot(const ReferenceSlot<GUM_SCALAR> &source)
        : ClassElement<GUM_SCALAR>(source), __slotType(source.__slotType),
          __isArray(source.__isArray) {
      GUM_CONS_CPY(ReferenceSlot);
      GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy constructor.");
    }

    // Copy operator. Raise a FatalError.
    template <typename GUM_SCALAR>
    ReferenceSlot<GUM_SCALAR> &ReferenceSlot<GUM_SCALAR>::
    operator=(const ReferenceSlot<GUM_SCALAR> &from) {
      GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy operator.");
    }

    template <typename GUM_SCALAR>
    INLINE typename ClassElement<GUM_SCALAR>::ClassElementType
    ReferenceSlot<GUM_SCALAR>::elt_type() const {
      return this->prm_refslot;
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR> &ReferenceSlot<GUM_SCALAR>::slotType() {
      return __slotType;
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElementContainer<GUM_SCALAR> &
    ReferenceSlot<GUM_SCALAR>::slotType() const {
      return __slotType;
    }

    template <typename GUM_SCALAR>
    INLINE bool ReferenceSlot<GUM_SCALAR>::isArray() const {
      return __isArray;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR> &ReferenceSlot<GUM_SCALAR>::type() {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR> &ReferenceSlot<GUM_SCALAR>::type() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR> &ReferenceSlot<GUM_SCALAR>::cpf() {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR> &ReferenceSlot<GUM_SCALAR>::cpf() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.");
    }

    template <typename GUM_SCALAR>
    INLINE void
    ReferenceSlot<GUM_SCALAR>::addParent(const ClassElement<GUM_SCALAR> &elt) {}

    template <typename GUM_SCALAR>
    INLINE void
    ReferenceSlot<GUM_SCALAR>::addChild(const ClassElement<GUM_SCALAR> &elt) {}

  } /* namespace prm */
} /* namespace gum */
