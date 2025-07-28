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
 * @brief Template implementations for the LeastSquareTestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy.h>

// =========================================================================


namespace gum {

  template < typename GUM_SCALAR >
  LeastSquareTestPolicy< GUM_SCALAR >::~LeastSquareTestPolicy() {
    for (auto obsIter = this->_obsTable_.cbeginSafe(); _obsTable_.cendSafe() != obsIter; ++obsIter)
      delete obsIter.val();

    GUM_DESTRUCTOR(LeastSquareTestPolicy);
  }

  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template < typename GUM_SCALAR >
  void LeastSquareTestPolicy< GUM_SCALAR >::addObservation(Idx attr, GUM_SCALAR value) {
    ITestPolicy< GUM_SCALAR >::addObservation(attr, value);
    _sumO_ += value;

    if (_sumAttrTable_.exists(attr)) _sumAttrTable_[attr] += value;
    else _sumAttrTable_.insert(attr, value);

    if (_nbObsTable_.exists(attr)) _nbObsTable_[attr]++;
    else _nbObsTable_.insert(attr, 1);

    if (!_obsTable_.exists(attr)) _obsTable_.insert(attr, new LinkedList< double >());
    _obsTable_[attr]->addLink(value);
  }

  // ############################################################################
  // @name Test result
  // ############################################################################

  // ============================================================================
  // Computes the GStat of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  void LeastSquareTestPolicy< GUM_SCALAR >::computeScore() {
    ITestPolicy< GUM_SCALAR >::computeScore();
    double mean         = _sumO_ / (double)this->nbObservation();
    double errorO       = 0.0;
    double sumErrorAttr = 0.0;
    for (auto attrIter = _sumAttrTable_.cbeginSafe(); attrIter != _sumAttrTable_.cendSafe();
         ++attrIter) {
      Idx    key       = attrIter.key();
      double meanAttr  = _sumAttrTable_[key] / (double)_nbObsTable_[key];
      double errorAttr = 0.0;

      const Link< double >* linky = _obsTable_[key]->list();
      while (linky) {
        errorAttr += std::pow(linky->element() - meanAttr, 2);
        errorO += std::pow(linky->element() - mean, 2);
        linky = linky->nextLink();
      }

      sumErrorAttr += ((double)_nbObsTable_[key] / (double)this->nbObservation()) * errorAttr;
    }
    _score_ = errorO - sumErrorAttr;
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  double LeastSquareTestPolicy< GUM_SCALAR >::score() {
    if (this->isModified_()) computeScore();
    return _score_;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template < typename GUM_SCALAR >
  double LeastSquareTestPolicy< GUM_SCALAR >::secondaryscore() const {
    if (this->isModified_()) computeScore();
    return _score_;
  }

  template < typename GUM_SCALAR >
  void LeastSquareTestPolicy< GUM_SCALAR >::add(const LeastSquareTestPolicy& src) {
    ITestPolicy< GUM_SCALAR >::add(src);

    for (auto obsIter = src.nbObsTable().cbeginSafe(); obsIter != src.nbObsTable().cendSafe();
         ++obsIter)
      if (_nbObsTable_.exists(obsIter.key())) _nbObsTable_[obsIter.key()] += obsIter.val();
      else _nbObsTable_.insert(obsIter.key(), obsIter.val());

    for (auto attrIter = src.sumAttrTable().cbeginSafe(); attrIter != src.sumAttrTable().cendSafe();
         ++attrIter)
      if (_sumAttrTable_.exists(attrIter.key())) _sumAttrTable_[attrIter.key()] += attrIter.val();
      else _sumAttrTable_.insert(attrIter.key(), attrIter.val());

    for (auto obsIter = src.obsTable().cbeginSafe(); obsIter != src.obsTable().cendSafe();
         ++obsIter) {
      if (!_obsTable_.exists(obsIter.key()))
        _obsTable_.insert(obsIter.key(), new LinkedList< double >());
      const Link< double >* srcLink = obsIter.val()->list();
      while (srcLink) {
        _obsTable_[obsIter.key()]->addLink(srcLink->element());
        srcLink = srcLink->nextLink();
      }
    }
  }

}   // End of namespace gum
