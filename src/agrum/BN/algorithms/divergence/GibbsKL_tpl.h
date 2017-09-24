/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief KL divergence between BNs -- implementation using Gibbs sampling
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>
#include <agrum/core/approximations/approximationScheme.h>
#include <agrum/core/hashTable.h>
#include <cmath>

#define GIBBSKL_DEFAULT_MAXITER 10000000
#define GIBBSKL_DEFAULT_EPSILON 1e-10
#define GIBBSKL_DEFAULT_MIN_EPSILON_RATE 1e-10
#define GIBBSKL_DEFAULT_PERIOD_SIZE 200
#define GIBBSKL_DEFAULT_VERBOSITY false
#define GIBBSKL_DEFAULT_BURNIN 2000
#define GIBBSKL_DEFAULT_TIMEOUT 6000

namespace gum {


  template <typename GUM_SCALAR>
  GibbsKL<GUM_SCALAR>::GibbsKL( const IBayesNet<GUM_SCALAR>& P,
                                const IBayesNet<GUM_SCALAR>& Q )
      : KL<GUM_SCALAR>( P, Q )
      , ApproximationScheme()
      , GibbsOperator<GUM_SCALAR>( P ) {
    GUM_CONSTRUCTOR( GibbsKL );

    setEpsilon( GIBBSKL_DEFAULT_EPSILON );
    setMinEpsilonRate( GIBBSKL_DEFAULT_MIN_EPSILON_RATE );
    setMaxIter( GIBBSKL_DEFAULT_MAXITER );
    setVerbosity( GIBBSKL_DEFAULT_VERBOSITY );
    setBurnIn( GIBBSKL_DEFAULT_BURNIN );
    setPeriodSize( GIBBSKL_DEFAULT_PERIOD_SIZE );
    setMaxTime( GIBBSKL_DEFAULT_TIMEOUT );
  }

  template <typename GUM_SCALAR>
  GibbsKL<GUM_SCALAR>::GibbsKL( const KL<GUM_SCALAR>& kl )
      : KL<GUM_SCALAR>( kl )
      , ApproximationScheme()
      // Gibbs operator with 10% of nodes changes at random between each samples
      , GibbsOperator<GUM_SCALAR>( kl.p(), 1 + ( kl.p().size() / 10 ), true ) {
    GUM_CONSTRUCTOR( GibbsKL );

    setEpsilon( GIBBSKL_DEFAULT_EPSILON );
    setMinEpsilonRate( GIBBSKL_DEFAULT_MIN_EPSILON_RATE );
    setMaxIter( GIBBSKL_DEFAULT_MAXITER );
    setVerbosity( GIBBSKL_DEFAULT_VERBOSITY );
    setBurnIn( GIBBSKL_DEFAULT_BURNIN );
    setPeriodSize( GIBBSKL_DEFAULT_PERIOD_SIZE );
    setMaxTime( GIBBSKL_DEFAULT_TIMEOUT );
  }

  template <typename GUM_SCALAR>
  GibbsKL<GUM_SCALAR>::~GibbsKL() {
    GUM_DESTRUCTOR( GibbsKL );
  }

  template <typename GUM_SCALAR>
  void GibbsKL<GUM_SCALAR>::_computeKL() {

    gum::Instantiation Iq;
    _q.completeInstantiation( Iq );


    gum::Instantiation I = this->monteCarloSample();
    initApproximationScheme();

    // map between particle() variables and _q variables (using name of vars)
    HashTable<const DiscreteVariable*, const DiscreteVariable*> map;

    for ( Idx ite = 0; ite < I.nbrDim(); ++ite ) {
      map.insert( &I.variable( ite ),
                  &_q.variableFromName( I.variable( ite ).name() ) );
    }

    // BURN IN
    for ( Idx i = 0; i < burnIn(); i++ )
      I = this->nextSample( I );

    // SAMPLING
    _klPQ = _klQP = _hellinger = (GUM_SCALAR)0.0;
    _errorPQ = _errorQP = 0;
    /// bool check_rate;
    GUM_SCALAR delta, ratio, error;
    delta = ratio = error = (GUM_SCALAR)-1;
    GUM_SCALAR oldPQ = 0.0;
    GUM_SCALAR pp, pq;

    do {

      this->disableMinEpsilonRate();
      I = this->nextSample( I );
      updateApproximationScheme();

      //_p.synchroInstantiations( Ip,I);
      Iq.setValsFrom( map, I );

      pp = _p.jointProbability( I );
      pq = _q.jointProbability( Iq );

      if ( pp != (GUM_SCALAR)0.0 ) {
        _hellinger += std::pow( std::sqrt( pp ) - std::sqrt( pq ), 2 ) / pp;

        if ( pq != (GUM_SCALAR)0.0 ) {
          _bhattacharya += std::sqrt( pq / pp );  // std::sqrt(pp*pq)/pp
          /// check_rate=true;
          this->enableMinEpsilonRate();  // replace check_rate=true;
          ratio = pq / pp;
          delta = (GUM_SCALAR)log2( ratio );
          _klPQ += delta;
        } else {
          _errorPQ++;
        }
      }

      if ( pq != (GUM_SCALAR)0.0 ) {
        if ( pp != (GUM_SCALAR)0.0 ) {
          // if we are here, it is certain that delta and ratio have been
          // computed
          // further lines above. (for now #112-113)
          _klQP += ( GUM_SCALAR )( -delta * ratio );
        } else {
          _errorQP++;
        }
      }

      if ( this->isEnabledMinEpsilonRate() ) {  // replace check_rate
        // delta is used as a temporary variable
        delta = _klPQ / nbrIterations();
        error = (GUM_SCALAR)std::abs( delta - oldPQ );
        oldPQ = delta;
      }
    } while ( continueApproximationScheme( error ) );  //

    _klPQ = -_klPQ / ( nbrIterations() );
    _klQP = -_klQP / ( nbrIterations() );
    _hellinger = std::sqrt( _hellinger / nbrIterations() );
    _bhattacharya = -std::log( _bhattacharya );
  }
}
