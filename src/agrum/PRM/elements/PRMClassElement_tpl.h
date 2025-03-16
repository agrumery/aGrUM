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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/**
 * @file
 * @brief Inline implementation of gum::PRMClassElement
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMClassElement< GUM_SCALAR >::PRMClassElement(const std::string& name) : PRMObject(name) {
      GUM_CONSTRUCTOR(PRMClassElement);
    }

    template < typename GUM_SCALAR >
    PRMClassElement< GUM_SCALAR >::PRMClassElement(const PRMClassElement< GUM_SCALAR >& source) :
        PRMObject(source.name()), _id_(source.id()) {
      GUM_CONS_CPY(PRMClassElement);
    }

    template < typename GUM_SCALAR >
    PRMClassElement< GUM_SCALAR >::~PRMClassElement() {
      GUM_DESTRUCTOR(PRMClassElement);
    }

    template < typename GUM_SCALAR >
    INLINE NodeId PRMClassElement< GUM_SCALAR >::id() const {
      return _id_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject::prm_type PRMClassElement< GUM_SCALAR >::obj_type() const {
      return prm_type::CLASS_ELT;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClassElement< GUM_SCALAR >::setId(NodeId id) {
      _id_ = id;
    }

    template < typename GUM_SCALAR >
    INLINE const std::string& PRMClassElement< GUM_SCALAR >::safeName() const {
      return safeName_;
    }

    template < typename GUM_SCALAR >
    INLINE std::string PRMClassElement< GUM_SCALAR >::cast(const PRMType& t) const {
      if (type().isSubTypeOf(t)) {
        return PRMObject::LEFT_CAST() + t.name() + PRMObject::RIGHT_CAST() + name();
      } else {
        GUM_ERROR(OperationNotAllowed, "illegal cast")
      }
    }

  } /* namespace prm */
} /* namespace gum */
