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
 * @brief A container for registering binary functions on
 * multiDimImplementations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

#  include <agrum/base/multidim/utils/operators/operatorRegister4MultiDim.h>

namespace gum {

  // adds a new entry into the register
  template < typename GUM_SCALAR >
  void OperatorRegister4MultiDim< GUM_SCALAR >::insert(
      const std::string&                                            operation_name,
      const std::string&                                            type1,
      const std::string&                                            type2,
      typename OperatorRegister4MultiDim< GUM_SCALAR >::OperatorPtr newFunction) {
    // insert the new entry
    OperatorSet* theset;

    if (!_set_.exists(operation_name)) {
      theset = _set_.insert(operation_name, new OperatorSet).second;
#  ifdef GUM_DEBUG_MODE
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the OperatorRegister4MultiDim will be
      // removed at the end of the program's execution.
      __debug__::_inc_deletion_("HashTable", __FILE__, __LINE__, "destructor of", (void*)theset);
#  endif /* GUM_DEBUG_MODE */
    } else {
      theset = _set_[operation_name];
    }

    std::pair< std::string, std::string > thepair(type1, type2);

    theset->insert(thepair, newFunction);
  }

  // removes a given entry from the register
  template < typename GUM_SCALAR >
  void OperatorRegister4MultiDim< GUM_SCALAR >::erase(const std::string& operation_name,
                                                      const std::string& type1,
                                                      const std::string& type2) {
    if (!_set_.exists(operation_name)) return;

    OperatorSet* theset = _set_[operation_name];

    theset->erase(std::pair< std::string, std::string >(type1, type2));
  }

  // indicates whether a given entry exists in the register
  template < typename GUM_SCALAR >
  INLINE bool OperatorRegister4MultiDim< GUM_SCALAR >::exists(const std::string& operation_name,
                                                              const std::string& type1,
                                                              const std::string& type2) const {
    if (!_set_.exists(operation_name)) return false;
    const OperatorSet&                          theset = *(_set_[operation_name]);
    const std::pair< std::string, std::string > key(type1, type2);
    return theset.exists(key);
  }

  /** @brief returns the specialized operator assigned to a given pair of
   * MultiDimImplementations */
  template < typename GUM_SCALAR >
  INLINE typename OperatorRegister4MultiDim< GUM_SCALAR >::OperatorPtr
      OperatorRegister4MultiDim< GUM_SCALAR >::get(const std::string& operation_name,
                                                   const std::string& type1,
                                                   const std::string& type2) const {
    const OperatorSet&                          theset = *(_set_[operation_name]);
    const std::pair< std::string, std::string > key(type1, type2);
    return theset[key];
  }

  // a named constructor that constructs one and only one Register per data
  // type
  template < typename GUM_SCALAR >
  OperatorRegister4MultiDim< GUM_SCALAR >& OperatorRegister4MultiDim< GUM_SCALAR >::Register() {
    // Here, this initialization is thread-safe due to Meyer’s Singleton property
    static OperatorRegister4MultiDim< GUM_SCALAR > container;

#  ifdef GUM_DEBUG_MODE
    static std::atomic_flag first = ATOMIC_FLAG_INIT;
    if (!first.test_and_set()) {
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the OperatorRegister4MultiDim will be
      // removed at the end of the program's execution.
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
  OperatorRegister4MultiDim< GUM_SCALAR >::OperatorRegister4MultiDim() {}

  // destructor
  template < typename GUM_SCALAR >
  OperatorRegister4MultiDim< GUM_SCALAR >::~OperatorRegister4MultiDim() {
    // remove all the sets
    for (typename HashTable< std::string, OperatorSet* >::iterator_safe iter = _set_.beginSafe();
         iter != _set_.endSafe();
         ++iter)
      delete iter.val();
  }

  // a function to more easily register new operators in MultiDims
  template < typename GUM_SCALAR >
  void registerOperator(const std::string& operation_name,
                        const std::string& type1,
                        const std::string& type2,
                        typename OperatorRegister4MultiDim< GUM_SCALAR >::OperatorPtr function) {
    OperatorRegister4MultiDim< GUM_SCALAR >::Register().insert(operation_name,
                                                               type1,
                                                               type2,
                                                               function);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
