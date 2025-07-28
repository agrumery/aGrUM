/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inline implementation of gum::PRMReferenceSlot
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/elements/PRMReferenceSlot.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >::PRMReferenceSlot(const std::string&                      name,
                                                     PRMClassElementContainer< GUM_SCALAR >& type,
                                                     bool isArray) :
        PRMClassElement< GUM_SCALAR >(name), _slotType_(type), _isArray_(isArray) {
      GUM_CONSTRUCTOR(PRMReferenceSlot);
      this->safeName_ = PRMObject::LEFT_CAST() + type.name() + PRMObject::RIGHT_CAST() + name;
    }

    // Destructor.
    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >::~PRMReferenceSlot() {
      GUM_DESTRUCTOR(PRMReferenceSlot);
    }

    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >::PRMReferenceSlot(const PRMReferenceSlot< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source), _slotType_(source._slotType_),
        _isArray_(source._isArray_) {
      GUM_CONS_CPY(PRMReferenceSlot);
      GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy constructor.")
    }

    // Copy operator. Raise a FatalError.
    template < typename GUM_SCALAR >
    PRMReferenceSlot< GUM_SCALAR >&
        PRMReferenceSlot< GUM_SCALAR >::operator=(const PRMReferenceSlot< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError, "illegal call to gum::ReferenceSlot copy operator.")
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMClassElement< GUM_SCALAR >::ClassElementType
        PRMReferenceSlot< GUM_SCALAR >::elt_type() const {
      return this->prm_refslot;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >& PRMReferenceSlot< GUM_SCALAR >::slotType() {
      return _slotType_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElementContainer< GUM_SCALAR >&
                 PRMReferenceSlot< GUM_SCALAR >::slotType() const {
      return _slotType_;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMReferenceSlot< GUM_SCALAR >::isArray() const {
      return _isArray_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRMReferenceSlot< GUM_SCALAR >::type() {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.")
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRMReferenceSlot< GUM_SCALAR >::type() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.")
    }

    template < typename GUM_SCALAR >
    INLINE Tensor< GUM_SCALAR >& PRMReferenceSlot< GUM_SCALAR >::cpf() {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.")
    }

    template < typename GUM_SCALAR >
    INLINE const Tensor< GUM_SCALAR >& PRMReferenceSlot< GUM_SCALAR >::cpf() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.")
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >* PRMReferenceSlot< GUM_SCALAR >::getCastDescendant() const {
      GUM_ERROR(OperationNotAllowed, "This is a ReferenceSlot.")
    }

    template < typename GUM_SCALAR >
    INLINE void
        PRMReferenceSlot< GUM_SCALAR >::addParent(const PRMClassElement< GUM_SCALAR >& elt) {}

    template < typename GUM_SCALAR >
    INLINE void PRMReferenceSlot< GUM_SCALAR >::addChild(const PRMClassElement< GUM_SCALAR >& elt) {
    }

  } /* namespace prm */
} /* namespace gum */
