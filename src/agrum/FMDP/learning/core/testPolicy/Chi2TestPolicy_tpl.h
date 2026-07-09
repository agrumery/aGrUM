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
 * @brief Template implementations for the Chi2TestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/testPolicy/Chi2TestPolicy.h>

// =========================================================================


namespace gum {


  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template < typename GUM_ELEMENT >
  void Chi2TestPolicy< GUM_ELEMENT >::addObservation(Idx iattr, GUM_ELEMENT ivalue) {
    ITestPolicy< GUM_ELEMENT >::addObservation(iattr, ivalue);
    _conTab_.add(iattr, ivalue);
  }

  // ############################################################################
  // @name Test result
  // ############################################################################

  // ============================================================================
  // Computes the GStat of current variable according to the test
  // ============================================================================
  template < typename GUM_ELEMENT >
  void Chi2TestPolicy< GUM_ELEMENT >::computeScore() const {
    ITestPolicy< GUM_ELEMENT >::computeScore();
    _chi2Score_ = 0;
    for (auto attrIter = _conTab_.attrABeginSafe(); attrIter != _conTab_.attrAEndSafe();
         ++attrIter) {
      double semiExpected = (double)(attrIter.val()) / (double)this->nbObservation();
      for (auto valIter = _conTab_.attrBBeginSafe(); valIter != _conTab_.attrBEndSafe();
           ++valIter) {
        double cell = (double)_conTab_.joint(attrIter.key(), valIter.key());
        if (cell < 5) continue;
        double expected = semiExpected * (double)(valIter.val());

        _chi2Score_ += std::pow(cell - expected, 2.0) / expected;
      }
    }
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template < typename GUM_ELEMENT >
  double Chi2TestPolicy< GUM_ELEMENT >::score() const {
    if (this->isModified_()) computeScore();
    double score = 1
                 - ChiSquare::probaChi2(_chi2Score_,
                                        (_conTab_.attrASize() - 1) * (_conTab_.attrBSize() - 1));
    return score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template < typename GUM_ELEMENT >
  double Chi2TestPolicy< GUM_ELEMENT >::secondaryscore() const {
    if (this->isModified_()) computeScore();
    return _chi2Score_;
  }

  template < typename GUM_ELEMENT >
  void Chi2TestPolicy< GUM_ELEMENT >::add(const Chi2TestPolicy< GUM_ELEMENT >& src) {
    ITestPolicy< GUM_ELEMENT >::add(src);
    _conTab_ += src.ct();
  }

  template < typename GUM_ELEMENT >
  Chi2TestPolicy< GUM_ELEMENT >::Chi2TestPolicy() :
      ITestPolicy< GUM_ELEMENT >(), _conTab_(), _chi2Score_(0) {
    GUM_CONSTRUCTOR(Chi2TestPolicy);
  }

  template < typename GUM_ELEMENT >
  Chi2TestPolicy< GUM_ELEMENT >::~Chi2TestPolicy() {
    GUM_DESTRUCTOR(Chi2TestPolicy);
    ;
  }

  template < typename GUM_ELEMENT >
  void* Chi2TestPolicy< GUM_ELEMENT >::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  template < typename GUM_ELEMENT >
  void Chi2TestPolicy< GUM_ELEMENT >::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(Chi2TestPolicy));
  }

  template < typename GUM_ELEMENT >
  bool Chi2TestPolicy< GUM_ELEMENT >::isTestRelevant() const {
    return (this->nbObservation() > 20 && this->nbObservation() > _conTab_.attrASize() * 5);
  }

  template < typename GUM_ELEMENT >
  const ContingencyTable< Idx, GUM_ELEMENT >& Chi2TestPolicy< GUM_ELEMENT >::ct() const {
    return _conTab_;
  }

  template < typename GUM_ELEMENT >
  std::string Chi2TestPolicy< GUM_ELEMENT >::toString() const {
    return std::format("{}\t\t\tContingency Table : \n{}\n\t\t\tGStat : {}\n\t\t\tGStat : {}\n",
                       ITestPolicy< GUM_ELEMENT >::toString(),
                       _conTab_.toString(),
                       this->score(),
                       this->secondaryscore());
  }

}   // End of namespace gum
