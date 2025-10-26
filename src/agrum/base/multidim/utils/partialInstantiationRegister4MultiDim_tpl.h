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
#pragma once


/**
 * @file
 * @brief Implementation of PartialInstantiationRegister4MultiDim.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

#  include <agrum/base/multidim/utils/partialInstantiationRegister4MultiDim.h>

namespace gum {

  // adds a new entry into the register
  template < typename GUM_SCALAR >
  void PartialInstantiationRegister4MultiDim< GUM_SCALAR >::insert(
      const std::string& instantiation_func_name,
      const std::string& type_multidim,
      typename PartialInstantiationRegister4MultiDim< GUM_SCALAR >::PartialInstantiationPtr
          newFunction) {
    // insert the new entry
    PartialInstantiationSet* theset = nullptr;

    if (!_set_.exists(instantiation_func_name)) {
      theset = _set_.insert(instantiation_func_name, new PartialInstantiationSet).second;
#  ifdef GUM_DEBUG_MODE
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the
      // PartialInstantiationRegister4MultiDim
      // will be removed at the end of the program's execution.
      __debug__::_inc_deletion_("HashTable", __FILE__, __LINE__, "destructor of", (void*)theset);
#  endif /* GUM_DEBUG_MODE */
    } else {
      theset = _set_[instantiation_func_name];
    }

    theset->insert(type_multidim, newFunction);
  }

  // removes a given entry from the register
  template < typename GUM_SCALAR >
  void PartialInstantiationRegister4MultiDim< GUM_SCALAR >::erase(
      const std::string& instantiation_func_name,
      const std::string& type_multidim) {
    if (!_set_.exists(instantiation_func_name)) return;

    PartialInstantiationSet* theset = _set_[instantiation_func_name];

    theset->erase(type_multidim);
  }

  // indicates whether a given entry exists in the register
  template < typename GUM_SCALAR >
  INLINE bool PartialInstantiationRegister4MultiDim< GUM_SCALAR >::exists(
      const std::string& instantiation_func_name,
      const std::string& type_multidim) const {
    if (!_set_.exists(instantiation_func_name)) return false;

    const PartialInstantiationSet& theset = *(_set_[instantiation_func_name]);
    return theset.exists(type_multidim);
  }

  /** @brief returns the specialized operator assigned to a given subtype of
   * MultiDimImplementation */
  template < typename GUM_SCALAR >
  INLINE typename PartialInstantiationRegister4MultiDim< GUM_SCALAR >::PartialInstantiationPtr
      PartialInstantiationRegister4MultiDim< GUM_SCALAR >::get(
          const std::string& instantiation_func_name,
          const std::string& type_multidim) const {
    const PartialInstantiationSet& theset = *(_set_[instantiation_func_name]);
    return theset[type_multidim];
  }

  // a named constructor that constructs one and only one Register per data
  // type
  template < typename GUM_SCALAR >
  PartialInstantiationRegister4MultiDim< GUM_SCALAR >&
      PartialInstantiationRegister4MultiDim< GUM_SCALAR >::Register() {
    // Here, this initialization is thread-safe due to Meyer’s Singleton property
    static PartialInstantiationRegister4MultiDim container;

#  ifdef GUM_DEBUG_MODE
    static std::atomic_flag first = ATOMIC_FLAG_INIT;
    if (!first.test_and_set()) {
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the
      // PartialInstantiationRegister4MultiDim
      // will be removed at the end of the program's execution.
      __debug__::_inc_deletion_("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&container._set_);
    }
#  endif /* GUM_DEBUG_MODE */

    return container;
  }

  // Default constructor: creates an empty register
  template < typename GUM_SCALAR >
  PartialInstantiationRegister4MultiDim< GUM_SCALAR >::PartialInstantiationRegister4MultiDim() {}

  // destructor
  template < typename GUM_SCALAR >
  PartialInstantiationRegister4MultiDim< GUM_SCALAR >::~PartialInstantiationRegister4MultiDim() {
    // remove all the sets
    for (typename HashTable< std::string, PartialInstantiationSet* >::iterator_safe iter
         = _set_.beginSafe();
         iter != _set_.endSafe();
         ++iter)
      delete iter.val();
  }

  // a function to more easily register new instantiation functions in
  // MultiDims
  template < typename GUM_SCALAR >
  void registerPartialInstantiation(
      const std::string& instantiation_func_name,
      const std::string& type_multidim,
      typename PartialInstantiationRegister4MultiDim< GUM_SCALAR >::PartialInstantiationPtr
          function) {
    PartialInstantiationRegister4MultiDim< GUM_SCALAR >::Register().insert(instantiation_func_name,
                                                                           type_multidim,
                                                                           function);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
