/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief Implementation of MultiDimPartialInstantiation.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <typeinfo>

#include <agrum/agrum.h>

#include <agrum/base/multidim/implementations/multiDimPartialInstantiation.h>   // to ease IDE parser

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // constructor
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::MultiDimPartialInstantiation() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimPartialInstantiation);
  }

  // copy constructor
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::MultiDimPartialInstantiation(
      const MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >& from) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimPartialInstantiation);
  }

  // destructor
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::~MultiDimPartialInstantiation() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimPartialInstantiation);
  }

  // virtual constructor
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >*
      MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::newFactory() const {
    return new MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >(*this);
  }

  // copy operator
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >&
      MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::operator=(
          const MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >& from) = default;

  // creates and returns the partial instantiation of the table
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  TABLE< GUM_ELEMENT >* MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::instantiate(
      const TABLE< GUM_ELEMENT >&                      table,
      const HashTable< const DiscreteVariable*, Idx >& inst_vars) {
    MultiDimImplementation< GUM_ELEMENT >* new_impl = partialInstantiation(table, inst_vars);
    return new TABLE< GUM_ELEMENT >(new_impl);
  }

  // creates and returns the partial instantiation of the table
  template < typename GUM_ELEMENT, template < typename > class TABLE >
  void MultiDimPartialInstantiation< GUM_ELEMENT, TABLE >::instantiate(
      TABLE< GUM_ELEMENT >&                            container,
      const TABLE< GUM_ELEMENT >&                      table,
      const HashTable< const DiscreteVariable*, Idx >& inst_vars) {
    MultiDimImplementation< GUM_ELEMENT >* new_impl = partialInstantiation(table, inst_vars);
    container                                       = *new_impl;
    delete new_impl;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
