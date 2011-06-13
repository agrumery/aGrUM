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

#define DEFAULT_MAXITER 10000000
#define DEFAULT_EPSILON 1e-4*log(2)
#define DEFAULT_MIN_EPSILON_RATE 1e-4
#define DEFAULT_PERIODE_SIZE 500
#define DEFAULT_VERBOSITY false

namespace gum {

  template<typename T_DATA>
  GibbsKL<T_DATA>::GibbsKL( const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q ) :
      GibbsSettings( DEFAULT_EPSILON,DEFAULT_MIN_EPSILON_RATE,DEFAULT_MAXITER,DEFAULT_VERBOSITY,DEFAULT_BURNIN,DEFAULT_PERIODE_SIZE ),
      KL<T_DATA> ( P,Q ),
      particle::Gibbs<T_DATA> ( P ) {
    GUM_CONSTRUCTOR( GibbsKL );
  }

  template<typename T_DATA>
  GibbsKL<T_DATA>::GibbsKL( const KL< T_DATA >& kl ) :
      GibbsSettings( DEFAULT_EPSILON,DEFAULT_MIN_EPSILON_RATE,DEFAULT_MAXITER,DEFAULT_VERBOSITY,DEFAULT_BURNIN,DEFAULT_PERIODE_SIZE ),
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
    _klPQ=_klQP=_hellinger=0.0;
    _errorPQ=_errorQP=0;

    {
      Instantiation Ip=_p.completeInstantiation();
      Instantiation Iq=_q.completeInstantiation();

      for ( Ip.setFirst();!Ip.end();++Ip ) {
        KL<T_DATA>::__synchroInstantiations( Iq,Ip );

        GUM_TRACE( Ip<<" : p="<<_p.jointProbability( Ip )<<"  q="<<_q.jointProbability( Iq ) );
      }
    }

    gum::Instantiation Iq=_q.completeInstantiation();

    initParticle();

    //BURN IN

    for ( Idx i = 0;i < burnIn();i++ ) nextParticle( );

    // SAMPLING
    double err = 10.0, last_err = 0.0, err_rate = 10.0;

    double tmp;

    Size nb_iter;

    for ( nb_iter = 1;nb_iter <= maxIter();nb_iter++ ) {
      nextParticle( );
      KL<T_DATA>::__synchroInstantiations( Iq,particle() );

      GUM_TRACE( particle()<<" - "<<Iq );

      T_DATA pp=_p.jointProbability( particle() );
      T_DATA pq=_q.jointProbability( Iq );

      if ( pp!=0.0 ) {
        _hellinger+=pow( sqrt( pp )-sqrt( pq ),2 ) /pp;

        if ( pq!=0.0 ) {
          _klPQ-=log2( pq/pp );

        } else {
          _errorPQ++;
        }
      }


      if ( pq!=0.0 ) {
        if ( pp!=0.0 ) {
          tmp=pp/pq;
          _klQP+= log2( tmp )/tmp;
        } else {
          _errorQP++;
        }
      }
    }

    if ( --nb_iter==0 ) GUM_ERROR( OperationNotAllowed,"no iteration at all" );

    _klPQ=-_klPQ/( nb_iter );

    _klQP=-_klQP/( nb_iter );

    _hellinger=sqrt( _hellinger/nb_iter );
  }

} // namespace gum
// kate: indent-mode cstyle; space-indent on; indent-width 2;
