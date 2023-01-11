/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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
  template < typename GUM_SCALAR >
  void Chi2TestPolicy< GUM_SCALAR >::addObservation(Idx iattr, GUM_SCALAR ivalue) {
    ITestPolicy< GUM_SCALAR >::addObservation(iattr, ivalue);
    _conTab_.add(iattr, ivalue);
  }


  // ############################################################################
  // @name Test result
  // ############################################################################

  // ============================================================================
  // Computes the GStat of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  void Chi2TestPolicy< GUM_SCALAR >::computeScore() const {
    ITestPolicy< GUM_SCALAR >::computeScore();
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
  template < typename GUM_SCALAR >
  double Chi2TestPolicy< GUM_SCALAR >::score() const {
    if (this->isModified_()) computeScore();
    double score
       = 1
       - ChiSquare::probaChi2(_chi2Score_, (_conTab_.attrASize() - 1) * (_conTab_.attrBSize() - 1));
    return score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template < typename GUM_SCALAR >
  double Chi2TestPolicy< GUM_SCALAR >::secondaryscore() const {
    if (this->isModified_()) computeScore();
    return _chi2Score_;
  }

  template < typename GUM_SCALAR >
  void Chi2TestPolicy< GUM_SCALAR >::add(const Chi2TestPolicy< GUM_SCALAR >& src) {
    ITestPolicy< GUM_SCALAR >::add(src);
    _conTab_ += src.ct();
  }

}   // End of namespace gum
