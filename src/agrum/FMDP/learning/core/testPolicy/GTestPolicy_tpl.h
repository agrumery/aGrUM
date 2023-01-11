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
 * @brief Template implementations for the GTestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy.h>
// =========================================================================


namespace gum {


  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template < typename GUM_SCALAR >
  void GTestPolicy< GUM_SCALAR >::addObservation(Idx iattr, GUM_SCALAR ivalue) {
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
  void GTestPolicy< GUM_SCALAR >::computeScore() const {
    ITestPolicy< GUM_SCALAR >::computeScore();
    _GStat_ = 0;

    // Itération sur l'axe 1 du tableau
    for (auto attrIter = _conTab_.attrABeginSafe(); attrIter != _conTab_.attrAEndSafe();
         ++attrIter) {
      // Mise en cache de valeur utile
      double semiExpected = (double)(attrIter.val()) / (double)this->nbObservation();

      // Itération sur l'axe 2 du tableau
      for (auto valIter = _conTab_.attrBBeginSafe(); valIter != _conTab_.attrBEndSafe();
           ++valIter) {
        // Récupération de la valeur en cellule
        Idx cell = _conTab_.joint(attrIter.key(), valIter.key());
        if (cell < 5) continue;

        // Récupération de la valeur en axe 2
        double expected = semiExpected * (double)valIter.val();

        _GStat_ += 2 * cell * log(cell / expected);
      }
    }
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template < typename GUM_SCALAR >
  double GTestPolicy< GUM_SCALAR >::score() const {
    if (this->isModified_()) computeScore();
    //      std::cout << this->toString() << std::endl;
    double score
       = 1 - ChiSquare::probaChi2(_GStat_, (_conTab_.attrASize() - 1) * (_conTab_.attrBSize() - 1));
    return score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template < typename GUM_SCALAR >
  double GTestPolicy< GUM_SCALAR >::secondaryscore() const {
    if (this->isModified_()) computeScore();
    return _GStat_;
  }

  template < typename GUM_SCALAR >
  void GTestPolicy< GUM_SCALAR >::add(const GTestPolicy< GUM_SCALAR >& src) {
    ITestPolicy< GUM_SCALAR >::add(src);
    _conTab_ += src.ct();
  }

}   // End of namespace gum
