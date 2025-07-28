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
 * @brief Implementation of gum::PRMParameter.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMParameter< GUM_SCALAR >::PRMParameter(const std::string& name,
                                             ParameterType      type,
                                             GUM_SCALAR         value) :
        PRMClassElement< GUM_SCALAR >(name), _type_(type), _value_(value) {
      GUM_CONSTRUCTOR(PRMParameter);
      this->safeName_ = name;
    }

    template < typename GUM_SCALAR >
    PRMParameter< GUM_SCALAR >::PRMParameter(const PRMParameter< GUM_SCALAR >& source) :
        PRMClassElement< GUM_SCALAR >(source.name()), _type_(source.valueType()),
        _value_(source.value()) {
      GUM_ERROR(OperationNotAllowed, "cannot copy Paramter<GUM_SCALAR")
    }

    template < typename GUM_SCALAR >
    PRMParameter< GUM_SCALAR >&
        PRMParameter< GUM_SCALAR >::operator=(const PRMParameter< GUM_SCALAR >& from) {
      GUM_ERROR(OperationNotAllowed, "cannot copy Paramter<GUM_SCALAR")
    }

    template < typename GUM_SCALAR >
    PRMParameter< GUM_SCALAR >::~PRMParameter() {
      GUM_DESTRUCTOR(PRMParameter);
    }

    template < typename GUM_SCALAR >
    typename PRMClassElement< GUM_SCALAR >::ClassElementType
        PRMParameter< GUM_SCALAR >::elt_type() const {
      return PRMClassElement< GUM_SCALAR >::ClassElementType::prm_parameter;
    }

    template < typename GUM_SCALAR >
    GUM_SCALAR PRMParameter< GUM_SCALAR >::value() const {
      return _value_;
    }

    template < typename GUM_SCALAR >
    void PRMParameter< GUM_SCALAR >::value(GUM_SCALAR value) {
      _value_ = value;
    }

    template < typename GUM_SCALAR >
    typename PRMParameter< GUM_SCALAR >::ParameterType
        PRMParameter< GUM_SCALAR >::valueType() const {
      return _type_;
    }

    template < typename GUM_SCALAR >
    PRMType& PRMParameter< GUM_SCALAR >::type() {
      GUM_ERROR(OperationNotAllowed, "Parameters do not have a type")
    }

    template < typename GUM_SCALAR >
    const PRMType& PRMParameter< GUM_SCALAR >::type() const {
      GUM_ERROR(OperationNotAllowed, "Parameters do not have a type")
    }

    template < typename GUM_SCALAR >
    Tensor< GUM_SCALAR >& PRMParameter< GUM_SCALAR >::cpf() {
      GUM_ERROR(OperationNotAllowed, "Parameters do not have a cpf")
    }

    template < typename GUM_SCALAR >
    const Tensor< GUM_SCALAR >& PRMParameter< GUM_SCALAR >::cpf() const {
      GUM_ERROR(OperationNotAllowed, "Parameters do not have a cpf")
    }

    template < typename GUM_SCALAR >
    void PRMParameter< GUM_SCALAR >::addParent(const gum::prm::PRMClassElement< GUM_SCALAR >&) {
      GUM_ERROR(OperationNotAllowed, "Parameters can not have parents")
    }

    template < typename GUM_SCALAR >
    void PRMParameter< GUM_SCALAR >::addChild(const gum::prm::PRMClassElement< GUM_SCALAR >&) {
      GUM_ERROR(OperationNotAllowed, "Parameters can not have children")
    }

    template < typename GUM_SCALAR >
    PRMAttribute< GUM_SCALAR >* PRMParameter< GUM_SCALAR >::getCastDescendant() const {
      GUM_ERROR(OperationNotAllowed, "Parameters can have cast descendants")
    }

  } /* namespace prm */
}   // namespace gum
