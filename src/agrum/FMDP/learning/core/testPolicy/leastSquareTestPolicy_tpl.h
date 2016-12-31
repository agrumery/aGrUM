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
 * @brief Template implementations for the LeastSquareTestPolicy class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/testPolicy/leastSquareTestPolicy.h>
// =========================================================================


namespace gum {

  template <typename GUM_SCALAR>
  LeastSquareTestPolicy<GUM_SCALAR>::~LeastSquareTestPolicy() {
    for ( auto obsIter = this->__obsTable.cbeginSafe();
          __obsTable.cendSafe() != obsIter;
          ++obsIter )
      delete obsIter.val();

    GUM_DESTRUCTOR( LeastSquareTestPolicy )
  }


  // ##########################################################################
  //
  // ##########################################################################

  // ==========================================================================
  //
  // ==========================================================================
  template <typename GUM_SCALAR>
  void LeastSquareTestPolicy<GUM_SCALAR>::addObservation( Idx        attr,
                                                          GUM_SCALAR value ) {
    ITestPolicy<GUM_SCALAR>::addObservation( attr, value );
    __sumO += value;

    if ( __sumAttrTable.exists( attr ) )
      __sumAttrTable[attr] += value;
    else
      __sumAttrTable.insert( attr, value );

    if ( __nbObsTable.exists( attr ) )
      __nbObsTable[attr]++;
    else
      __nbObsTable.insert( attr, 1 );

    if ( !__obsTable.exists( attr ) )
      __obsTable.insert( attr, new LinkedList<double>() );
    __obsTable[attr]->addLink( value );
  }


  // ############################################################################
  // @name Test result
  // ############################################################################

  // ============================================================================
  // Computes the GStat of current variable according to the test
  // ============================================================================
  template <typename GUM_SCALAR>
  void LeastSquareTestPolicy<GUM_SCALAR>::computeScore() const {
    ITestPolicy<GUM_SCALAR>::computeScore();
    double mean = __sumO / (double)this->nbObservation();
    double errorO = 0.0;
    double sumErrorAttr = 0.0;
    for ( auto attrIter = __sumAttrTable.cbeginSafe();
          attrIter != __sumAttrTable.cendSafe();
          ++attrIter ) {
      Idx    key = attrIter.key();
      double meanAttr = __sumAttrTable[key] / (double)__nbObsTable[key];
      double errorAttr = 0.0;

      const Link<double>* linky = __obsTable[key]->list();
      while ( linky ) {
        errorAttr += std::pow( linky->element() - meanAttr, 2 );
        errorO += std::pow( linky->element() - mean, 2 );
        linky = linky->nextLink();
      }

      sumErrorAttr +=
          ( (double)__nbObsTable[key] / (double)this->nbObservation() ) *
          errorAttr;
    }
    __score = errorO - sumErrorAttr;
  }

  // ============================================================================
  // Returns the performance of current variable according to the test
  // ============================================================================
  template <typename GUM_SCALAR>
  double LeastSquareTestPolicy<GUM_SCALAR>::score() const {
    if ( this->_isModified() ) computeScore();
    return __score;
  }

  // ============================================================================
  // Returns a second criterion to severe ties
  // ============================================================================
  template <typename GUM_SCALAR>
  double LeastSquareTestPolicy<GUM_SCALAR>::secondaryscore() const {
    if ( this->_isModified() ) computeScore();
    return __score;
  }

  template <typename GUM_SCALAR>
  void LeastSquareTestPolicy<GUM_SCALAR>::add( const LeastSquareTestPolicy& src ) {
    ITestPolicy<GUM_SCALAR>::add( src );

    for ( auto obsIter = src.nbObsTable().cbeginSafe();
          obsIter != src.nbObsTable().cendSafe();
          ++obsIter )
      if ( __nbObsTable.exists( obsIter.key() ) )
        __nbObsTable[obsIter.key()] += obsIter.val();
      else
        __nbObsTable.insert( obsIter.key(), obsIter.val() );

    for ( auto attrIter = src.sumAttrTable().cbeginSafe();
          attrIter != src.sumAttrTable().cendSafe();
          ++attrIter )
      if ( __sumAttrTable.exists( attrIter.key() ) )
        __sumAttrTable[attrIter.key()] += attrIter.val();
      else
        __sumAttrTable.insert( attrIter.key(), attrIter.val() );

    for ( auto obsIter = src.obsTable().cbeginSafe();
          obsIter != src.obsTable().cendSafe();
          ++obsIter ) {
      if ( !__obsTable.exists( obsIter.key() ) )
        __obsTable.insert( obsIter.key(), new LinkedList<double>() );
      const Link<double>* srcLink = obsIter.val()->list();
      while ( srcLink ) {
        __obsTable[obsIter.key()]->addLink( srcLink->element() );
        srcLink = srcLink->nextLink();
      }
    }
  }

}  // End of namespace gum
