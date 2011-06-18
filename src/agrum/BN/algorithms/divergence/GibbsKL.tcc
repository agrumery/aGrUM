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
 * @brief KL divergence between BNs brute force implementation
 *
 * @author Pierre-Henri Wuillemin
 */
// ============================================================================
#include <math.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>
#include <agrum/BN/particles/Gibbs.h>
#include <agrum/BN/algorithms/approximationSettings.h>

#define KL_DEFAULT_MAXITER 10000000
#define KL_DEFAULT_EPSILON 1e-10
#define KL_DEFAULT_MIN_EPSILON_RATE 1e-10
#define KL_DEFAULT_PERIODE_SIZE 500
#define KL_DEFAULT_VERBOSITY false
#define KL_DEFAULT_BURNIN 3000

namespace gum {

  template<typename T_DATA>
  GibbsKL<T_DATA>::GibbsKL( const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q ) :
      GibbsSettings( KL_DEFAULT_EPSILON,
                     KL_DEFAULT_MIN_EPSILON_RATE,
                     KL_DEFAULT_MAXITER,KL_DEFAULT_VERBOSITY,
                     KL_DEFAULT_BURNIN,
                     KL_DEFAULT_PERIODE_SIZE ),
      KL<T_DATA> ( P,Q ),
      particle::Gibbs<T_DATA> ( P ) {
    GUM_CONSTRUCTOR( GibbsKL );
  }

  template<typename T_DATA>
  GibbsKL<T_DATA>::GibbsKL( const KL< T_DATA >& kl ) :
      GibbsSettings( KL_DEFAULT_EPSILON,
                     KL_DEFAULT_MIN_EPSILON_RATE,
                     KL_DEFAULT_MAXITER,
                     KL_DEFAULT_VERBOSITY,
                     KL_DEFAULT_BURNIN,
                     KL_DEFAULT_PERIODE_SIZE ),
      KL<T_DATA> ( kl ),
      particle::Gibbs<T_DATA> ( kl.p() )  {
    GUM_CONSTRUCTOR( GibbsKL );
  }

  template<typename T_DATA>
  GibbsKL<T_DATA>::~GibbsKL() {
    GUM_DESTRUCTOR( GibbsKL );
  }

  template<typename T_DATA>
  void GibbsKL<T_DATA>::_computeKL() {

    gum::Instantiation Iq;_q.completeInstantiation( Iq );
    gum::Instantiation Ip;_p.completeInstantiation( Ip );

    initParticle();
    initApproximationScheme();

    //BURN IN
    for ( Idx i = 0;i < burnIn();i++ ) nextParticle( );

    // SAMPLING
    _klPQ=_klQP=_hellinger=( T_DATA )0.0;
    _errorPQ=_errorQP=0;
    bool check_rate;
    T_DATA delta,ratio,error;
    delta=ratio=error=( T_DATA )-1;
    T_DATA oldPQ=0.0;
    T_DATA pp,pq;
    do {
      check_rate=false;

      for ( Size i=1;i<=periodeSize();i++ ) {
        nextParticle( );
        updateApproximationScheme();

        //_p.synchroInstantiations( Ip,particle() );
        _q.synchroInstantiations( Iq,particle() );

        pp=_p.jointProbability( particle() );
        pq=_q.jointProbability( Iq );

        if ( pp!=0.0 ) {
          _hellinger+=pow( sqrt( pp )-sqrt( pq ),2 ) /pp;

          if ( pq!=0.0 ) {
            check_rate=true;
            ratio=pq/pp;
            delta=( T_DATA ) log2( ratio );
            _klPQ+=delta;
          } else {
            _errorPQ++;
          }
        }

        if ( pq!=0.0 ) {
          if ( pp!=0.0 ) {
            // if we are here, it is certain that delta and ratio have been computed 
            // further lines above. (for now #112-113)
            _klQP+= ( T_DATA )( -delta*ratio );
          } else {
            _errorQP++;
          }
        }
      }

      if ( check_rate ) {
        // delta is used as a temporary variable
        delta=_klPQ/nbrIterations();
        error=( double )fabs( delta-oldPQ );
        oldPQ=delta;
      }
    } while ( testApproximationScheme( error ,check_rate )==APPROX_CONTINUE );

    /*if ( verbosity() ) {
      GUM_TRACE( messageApproximationScheme() );
      GUM_TRACE( "#iterations = "<<nbrIterations() );
    }*/

    _klPQ=-_klPQ/( nbrIterations() );
    _klQP=-_klQP/( nbrIterations() );
    _hellinger=sqrt( _hellinger/nbrIterations() );
  }

} // namespace gum
// kate: indent-mode cstyle; space-indent on; indent-width 2;
