/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
  template <typename GUM_SCALAR>
  void Chi2TestPolicy<GUM_SCALAR>::addObservation( Idx iattr, GUM_SCALAR ivalue ) {
    ITestPolicy<GUM_SCALAR>::addObservation( iattr, ivalue );
    __conTab.add( iattr, ivalue );
  }


  // ############################################################################
  // @name Test result
  // ############################################################################

  // ============================================================================
  // Computes the GStat of current variable according to the test
  // ============================================================================
  template <typename GUM_SCALAR>
  void Chi2TestPolicy<GUM_SCALAR>::computeScore() const {
    ITestPolicy<GUM_SCALAR>::computeScore();
    __chi2Score = 0;
    for ( auto attrIter = __conTab.attrABeginSafe();
          attrIter != __conTab.attrAEndSafe();
          ++attrIter ) {
      double semiExpected =
          (double)( attrIter.val() ) / (double)this->nbObservation();
      for ( auto valIter = __conTab.attrBBeginSafe();
            valIter != __conTab.attrBEndSafe();
            ++valIter ) {
        double cell = (double)__conTab.joint( attrIter.key(), valIter.key() );
        if ( cell < 5 ) continue;
        double expected = semiExpected * (double)( valIter.val() );

        __chi2Score += std::pow( cell - expected, 2.0 ) / expected;
      }
    }
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template <typename GUM_SCALAR>
  double Chi2TestPolicy<GUM_SCALAR>::score() const {
    if ( this->_isModified() ) computeScore();
    double score = 1 - ChiSquare::probaChi2( __chi2Score,
                                             ( __conTab.attrASize() - 1 ) *
                                                 ( __conTab.attrBSize() - 1 ) );
    return score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template <typename GUM_SCALAR>
  double Chi2TestPolicy<GUM_SCALAR>::secondaryscore() const {
    if ( this->_isModified() ) computeScore();
    return __chi2Score;
  }

  template <typename GUM_SCALAR>
  void Chi2TestPolicy<GUM_SCALAR>::add( const Chi2TestPolicy<GUM_SCALAR>& src ) {
    ITestPolicy<GUM_SCALAR>::add( src );
    __conTab += src.ct();
  }

}  // End of namespace gum
