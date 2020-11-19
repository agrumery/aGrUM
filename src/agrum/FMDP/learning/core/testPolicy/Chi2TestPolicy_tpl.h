/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
    conTab__.add(iattr, ivalue);
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
    chi2Score__ = 0;
    for (auto attrIter = conTab__.attrABeginSafe();
         attrIter != conTab__.attrAEndSafe();
         ++attrIter) {
      double semiExpected =
         (double)(attrIter.val()) / (double)this->nbObservation();
      for (auto valIter = conTab__.attrBBeginSafe();
           valIter != conTab__.attrBEndSafe();
           ++valIter) {
        double cell = (double)conTab__.joint(attrIter.key(), valIter.key());
        if (cell < 5) continue;
        double expected = semiExpected * (double)(valIter.val());

        chi2Score__ += std::pow(cell - expected, 2.0) / expected;
      }
    }
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  double Chi2TestPolicy< GUM_SCALAR >::score() const {
    if (this->isModified_()) computeScore();
    double score =
       1
       - ChiSquare::probaChi2(
          chi2Score__, (conTab__.attrASize() - 1) * (conTab__.attrBSize() - 1));
    return score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template < typename GUM_SCALAR >
  double Chi2TestPolicy< GUM_SCALAR >::secondaryscore() const {
    if (this->isModified_()) computeScore();
    return chi2Score__;
  }

  template < typename GUM_SCALAR >
  void Chi2TestPolicy< GUM_SCALAR >::add(const Chi2TestPolicy< GUM_SCALAR >& src) {
    ITestPolicy< GUM_SCALAR >::add(src);
    conTab__ += src.ct();
  }

}   // End of namespace gum
