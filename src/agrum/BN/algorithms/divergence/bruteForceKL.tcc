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
#include <agrum/BN/algorithms/divergence/bruteForceKL.h>

namespace gum {
  template<typename T_DATA>
  BruteForceKL<T_DATA>::BruteForceKL( const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q ) :KL<T_DATA> ( P,Q ) {
    GUM_CONSTRUCTOR( BruteForceKL );
  }

  template<typename T_DATA>
  BruteForceKL<T_DATA>::BruteForceKL( const KL< T_DATA >& kl ) :KL<T_DATA> ( kl ) {
    GUM_CONSTRUCTOR( BruteForceKL );
  }

  template<typename T_DATA>
  BruteForceKL<T_DATA>::~BruteForceKL() {
    GUM_DESTRUCTOR( BruteForceKL );
  }

  template<typename T_DATA>
  void BruteForceKL<T_DATA>::_computeKL() {
    _klPQ=_klQP=_hellinger=_bhattacharya=( T_DATA )0.0;
    _errorPQ=_errorQP=( T_DATA )0;

    gum::Instantiation Ip;_p.completeInstantiation( Ip );
    gum::Instantiation Iq;_q.completeInstantiation( Iq );

    for ( Ip.setFirst();! Ip.end();++Ip ) {
      _q.synchroInstantiations( Iq,Ip );
      T_DATA pp=_p.jointProbability( Ip );
      T_DATA pq=_q.jointProbability( Iq );

      _hellinger+=pow( sqrt( pp )-sqrt( pq ),2 );
      _bhattacharya+=sqrt( pp*pq );

      if ( pp!=( T_DATA )0.0 ) {
        if ( pq!=( T_DATA )0.0 ) {
          _klPQ-=pp*log2( pq/pp );
        } else {
          _errorPQ++;
        }
      }

      if ( pq!=( T_DATA )0.0 ) {
        if ( pp!=( T_DATA )0.0 ) {
          _klQP-=pq*log2( pp/pq );
        } else {
          _errorQP++;
        }
      }
    }
    _hellinger=sqrt( _hellinger );
    _bhattacharya=-log( _bhattacharya );
  }

} // namespace gum
