/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


/** @file
 * @brief Outlined implementation of ArgMaxSet
 *
 * @author Jean-Christophe MAGNAN
 */

#include <agrum/base/core/argMaxSet.h>

namespace gum {

  // ###########################################################################
  // CNL
  // ###########################################################################

  // ============================================================================
  // Constructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::ArgMaxSet() {
    GUM_CONSTRUCTOR(ArgMaxSet);
    _argMaxSeq_ = new Sequence< GUM_SCALAR_SEQ >();
  }

  // ============================================================================
  // Constructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::ArgMaxSet(const GUM_SCALAR_VAL& val,
                                                         const GUM_SCALAR_SEQ& elem) {
    GUM_CONSTRUCTOR(ArgMaxSet);
    _argMaxSeq_ = new Sequence< GUM_SCALAR_SEQ >();
    _argMaxSeq_->insert(elem);
    _val_ = val;
  }

  // ============================================================================
  // Copy Constructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::ArgMaxSet(
      const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src) {
    GUM_CONS_CPY(ArgMaxSet);
    _argMaxSeq_ = new Sequence< GUM_SCALAR_SEQ >();
    this->operator+=(src);
    _val_ = src.value();
  }

  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
      ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator=(
          const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src) {
    this->_argMaxSeq_->clear();
    this->operator+=(src);
    _val_ = src.value();
    return *this;
  }

  // ============================================================================
  // Destructor
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::~ArgMaxSet() {
    GUM_DESTRUCTOR(ArgMaxSet);
    delete _argMaxSeq_;
  }

  // ###########################################################################
  // Operators
  // ###########################################################################

  // ============================================================================
  // Ajout d'un élément
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
      ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator+=(const GUM_SCALAR_SEQ& elem) {
    _argMaxSeq_->insert(elem);
    return *this;
  }

  // ============================================================================
  // Use to insert the content of another set inside this one
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >&
      ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator+=(
          const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& src) {
    for (auto iter = src.beginSafe(); iter != src.endSafe(); ++iter)
      if (!_argMaxSeq_->exists(*iter)) _argMaxSeq_->insert(*iter);
    return *this;
  }

  // ============================================================================
  // Compares two ArgMaxSet to check if they are equals
  // ============================================================================
  template < typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ >
  bool ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >::operator==(
      const ArgMaxSet< GUM_SCALAR_VAL, GUM_SCALAR_SEQ >& compared) const {
    if (_val_ != compared.value()) return false;
    for (auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter)
      if (!_argMaxSeq_->exists(*iter)) return false;
    for (auto iter = this->beginSafe(); iter != this->endSafe(); ++iter)
      if (!compared.exists(*iter)) return false;
    return true;
  }

}   // End of namespace gum
