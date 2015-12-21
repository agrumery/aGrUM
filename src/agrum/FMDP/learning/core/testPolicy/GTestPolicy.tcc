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
 * @brief Template implementations for the GTestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy.h>
// =========================================================================


namespace gum {


  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template <typename GUM_SCALAR>
  void GTestPolicy<GUM_SCALAR>::addObservation( Idx iattr, GUM_SCALAR ivalue ) {
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
  void GTestPolicy<GUM_SCALAR>::computeScore() const {
    ITestPolicy<GUM_SCALAR>::computeScore();
    __GStat = 0;

    // Itération sur l'axe 1 du tableau
    for ( auto attrIter = __conTab.attrABeginSafe();
          attrIter != __conTab.attrAEndSafe();
          ++attrIter ) {

      // Mise en cache de valeur utile
      double semiExpected =
          (double)( attrIter.val() ) / (double)this->nbObservation();

      // Itération sur l'axe 2 du tableau
      for ( auto valIter = __conTab.attrBBeginSafe();
            valIter != __conTab.attrBEndSafe();
            ++valIter ) {

        // Récupération de la valeur en cellule
        double cell = __conTab.joint( attrIter.key(), valIter.key() );
        if ( cell < 5 ) continue;

        // Récupération de la valeur en axe 2
        double expected = semiExpected * (double)valIter.val();

        __GStat += 2 * cell * log( cell / expected );
      }
    }
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template <typename GUM_SCALAR>
  double GTestPolicy<GUM_SCALAR>::score() const {
    if ( this->_isModified() ) computeScore();
    //      std::cout << this->toString() << std::endl;
    double score = 1 - ChiSquare::probaChi2( __GStat,
                                             ( __conTab.attrASize() - 1 ) *
                                                 ( __conTab.attrBSize() - 1 ) );
    return score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template <typename GUM_SCALAR>
  double GTestPolicy<GUM_SCALAR>::secondaryscore() const {
    if ( this->_isModified() ) computeScore();
    return __GStat;
  }

  template <typename GUM_SCALAR>
  void GTestPolicy<GUM_SCALAR>::add( const GTestPolicy<GUM_SCALAR>& src ) {
    ITestPolicy<GUM_SCALAR>::add( src );
    __conTab += src.ct();
  }

}  // End of namespace gum


// =====================================================================
// Cache system not to be deleted


//  template < typename GUM_SCALAR >
//  HashTable<std::tuple<Idx,Idx,Idx>, double>
//  GTestPolicy<GUM_SCALAR>::__logCache;

//  template < typename GUM_SCALAR >
//  Idx GTestPolicy<GUM_SCALAR>::nbLog = 0;
//  template < typename GUM_SCALAR >
//  Idx GTestPolicy<GUM_SCALAR>::nbLogt = 0;


// template < typename GUM_SCALAR >
// void GTestPolicy<GUM_SCALAR>::addObservation( Idx iattr, GUM_SCALAR ivalue )
// {

//      std::tuple<Idx,Idx,Idx> cacheTuple;
//      std::get<2>(cacheTuple) = this->nbObservation();
//      Idx bMarg = __conTab.attrBMarginal(ivalue);
//      for ( auto attrIter = __conTab.attrABeginSafe(); attrIter !=
//      __conTab.attrAEndSafe(); ++attrIter ){
//        std::get<1>(cacheTuple) = bMarg*attrIter.val();
//        std::get<0>(cacheTuple) = __conTab.joint(attrIter.val(), bMarg);
//        if( __logCache.exists(cacheTuple) )
//          __GStat -= __logCache[cacheTuple];
//      }
//      Idx aMarg = __conTab.attrAMarginal(iattr);
//      for ( auto valIter = __conTab.attrBBeginSafe(); valIter !=
//      __conTab.attrBEndSafe(); ++valIter ){
//        std::get<1>(cacheTuple) = aMarg*valIter.val();
//        std::get<0>(cacheTuple) = __conTab.joint(aMarg, valIter.val());
//        if( __logCache.exists(cacheTuple) )
//          __GStat -= __logCache[cacheTuple];
//      }
//      std::get<1>(cacheTuple) = aMarg*bMarg;
//      std::get<0>(cacheTuple) = __conTab.joint(aMarg, bMarg);
//      if( __logCache.exists(cacheTuple) )
//        __GStat += __logCache[cacheTuple];


//  ITestPolicy<GUM_SCALAR>::addObservation(iattr, ivalue);
//  __conTab.add( iattr, ivalue );


//      ITestPolicy<GUM_SCALAR>::computeScore();

//      std::get<2>(cacheTuple) = this->nbObservation();
//      bMarg = __conTab.attrBMarginal(ivalue);
//      for ( auto attrIter = __conTab.attrABeginSafe(); attrIter !=
//      __conTab.attrAEndSafe(); ++attrIter ){
//        std::get<1>(cacheTuple) = bMarg*attrIter.val();
//        std::get<0>(cacheTuple) = __conTab.joint(attrIter.val(), bMarg);
//        if( !__logCache.exists(cacheTuple) )
//          __logEval(cacheTuple);
//        __GStat += __logCache[cacheTuple];
//      }
//      aMarg = __conTab.attrAMarginal(iattr);
//      for ( auto valIter = __conTab.attrBBeginSafe(); valIter !=
//      __conTab.attrBEndSafe(); ++valIter ){
//        std::get<1>(cacheTuple) = aMarg*valIter.val();
//        std::get<0>(cacheTuple) = __conTab.joint(aMarg, valIter.val());
//        if( !__logCache.exists(cacheTuple) )
//          __logEval(cacheTuple);
//        __GStat += __logCache[cacheTuple];
//      }
//      std::get<1>(cacheTuple) = aMarg*bMarg;
//      std::get<0>(cacheTuple) = __conTab.joint(aMarg, bMarg);
//      if( std::get<0>(cacheTuple) >= 5)
//        __GStat -= __logCache[cacheTuple];
//}


// template < typename GUM_SCALAR >
// void GTestPolicy<GUM_SCALAR>::computeScore() const {
//  ITestPolicy<GUM_SCALAR>::computeScore();
//  __GStat = 0;
//  std::tuple<Idx,Idx,Idx> cacheTuple;
//  std::get<2>(cacheTuple) = this->nbObservation();

// Itération sur l'axe 1 du tableau
//  for ( auto attrIter = __conTab.attrABeginSafe(); attrIter !=
//  __conTab.attrAEndSafe(); ++attrIter ){

// Mise en cache de valeur utile
//        Idx semiExpected = attrIter.val();
//        std::get<1>(cacheTuple) = (attrIter.val());
//        double semiExpected = (double)(std::get<1>(cacheTuple))/(double)
//        std::get<3>(cacheTuple);

// Itération sur l'axe 2 du tableau
//    for ( auto valIter = __conTab.attrBBeginSafe(); valIter !=
//    __conTab.attrBEndSafe(); ++valIter ) {

// Récupération de la valeur en cellule
//      std::get<0>(cacheTuple) = __conTab.joint(attrIter.key(),valIter.key());
//      if( std::get<0>(cacheTuple) < 5 )
//        continue;

// Récupération de la valeur en axe 2
//      std::get<1>(cacheTuple) = attrIter.val() * valIter.val();

//          ++nbLogt;
//          if( !__logCache.exists(cacheTuple) )
//            __logEval(cacheTuple);
//          else
//            ++nbLog;
//          __GStat += __logCache[cacheTuple];
//      __GStat += __logEval(cacheTuple);
//    }
//  }
//      std::cout << "LOG : " << nbLog << " / " << nbLogt << std::endl;
//}


// template < typename GUM_SCALAR >
// double GTestPolicy<GUM_SCALAR>::__logEval(std::tuple<Idx,Idx,Idx> values){
//  double logValue = 0.0;
////      if(std::get<0>(values) >= 5 ){
//    double cell = std::get<0>(values);
//    double expected = std::get<1>(values);
//    double tot = std::get<2>(values);
//    logValue = 2*cell*log(cell*tot/expected); //std::round(* std::pow(10, 8))
//    / std::pow(10, 8);
////      }
////      __logCache.insert( values,  logValue);
//  return logValue;
//}
