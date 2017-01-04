/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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
 * @brief Implementation of Loopy Belief Propagation in Bayesian Networks.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <algorithm>
#include <sstream>
#include <string>

#define LBP_DEFAULT_MAXITER 10000000
#define LBP_DEFAULT_EPSILON 1e-4 * std::log( 2 )
#define LBP_DEFAULT_MIN_EPSILON_RATE 1e-4
#define LBP_DEFAULT_PERIOD_SIZE 500
#define LBP_DEFAULT_VERBOSITY false


// to ease parsing for IDE
#include <agrum/BN/inference/loopyBeliefPropagation.h>


namespace gum {

  /// default constructor
  template <typename GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::LoopyBeliefPropagation(
      const IBayesNet<GUM_SCALAR>* BN )
      : ApproximationScheme()
      , MarginalTargetedInference<GUM_SCALAR>( BN ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( LoopyBeliefPropagation );

    setEpsilon( LBP_DEFAULT_EPSILON );
    setMinEpsilonRate( LBP_DEFAULT_MIN_EPSILON_RATE );
    setMaxIter( LBP_DEFAULT_MAXITER );
    setVerbosity( LBP_DEFAULT_VERBOSITY );
    setPeriodSize( LBP_DEFAULT_PERIOD_SIZE );

    __init_messages();
  }

  /// destructor
  template <typename GUM_SCALAR>
  INLINE LoopyBeliefPropagation<GUM_SCALAR>::~LoopyBeliefPropagation() {
    GUM_DESTRUCTOR( LoopyBeliefPropagation );
  }


  template <typename GUM_SCALAR>
  void LoopyBeliefPropagation<GUM_SCALAR>::__init_messages() {
    __messages.clear();
    for ( const auto& tail : this->BN().nodes() ) {
      Potential<GUM_SCALAR> p;
      p.add( this->BN().variable( tail ) );
      p.fill( static_cast<GUM_SCALAR>( 1 ) );

      for ( const auto& head : this->BN().dag().children( tail ) ) {
        __messages.insert( Arc( head, tail ), p );
        __messages.insert( Arc( tail, head ), p );
        __messages[Arc( tail, head )].normalize();
      }
    }
  }

  template <typename GUM_SCALAR>
  void LoopyBeliefPropagation<GUM_SCALAR>::_updateOutdatedBNStructure() {
    __init_messages();
    GUM_CHECKPOINT;
  }

  /// Returns the probability of the variable.
  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  LoopyBeliefPropagation<GUM_SCALAR>::_posterior( NodeId id ) {
    // return __sampling_nbr[id];
  }


  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdPi( NodeId node ) {
    auto piX = this->BN().cpt( node );
    for ( const auto& par : this->BN().dag().parents( node ) ) {
      piX *= __messages[Arc( par, node )];
    }
    return piX;
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdPi( NodeId node,
                                                       NodeId except ) {
    auto piX = this->BN().cpt( node );
    for ( const auto& par : this->BN().dag().parents( node ) ) {
      if ( par != except ) {
        piX *= __messages[Arc( par, node )];
      }
    }
    return piX;
  }


  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdLambda( NodeId node ) {
    Potential<GUM_SCALAR> lamX;
    if ( this->hasEvidence( node ) ) {
      lamX = *(this->evidence()[node]);
    } else {
      lamX.add( this->BN().variable( node ) );
      lamX.fill( 1 );
    }
    for ( const auto& child : this->BN().dag().children( node ) ) {
      lamX *= __messages[Arc( child, node )];
    }
    return lamX;
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdLambda( NodeId node,
                                                           NodeId except ) {
    Potential<GUM_SCALAR> lamX;
    if ( this->hasEvidence( node ) ) {
      lamX = this->evidence()[node];
    } else {
      lamX.add( this->BN().variable( node ) );
      lamX.fill( 1 );
    }
    for ( const auto& child : this->BN().dag().children( node ) ) {
      if ( child != except ) {
        lamX *= __messages[Arc( child, node )];
      }
    }
    return lamX;
  }


  template <typename GUM_SCALAR>
  GUM_SCALAR
  LoopyBeliefPropagation<GUM_SCALAR>::__updateNodeMessage( NodeId nodeX ) {
    const auto& varX = this->BN().variable( nodeX );

    auto piX = __computeProdPi( nodeX );
    piX = piX.margSumIn( {&varX} );

    auto lamX = __computeProdLambda( nodeX );

    GUM_SCALAR KL = 0;

    for ( const auto& par : this->BN().dag().parents( nodeX ) ) {
      auto newLambda = ( __computeProdPi( nodeX, par )
                             .margSumIn( {&varX, &this->BN().variable( par )} ) *
                         lamX )
                           .margSumOut( {&varX} );
    }
  }

  template <typename GUM_SCALAR>
  INLINE void LoopyBeliefPropagation<GUM_SCALAR>::__initStats() {}


  /// Returns the probability of the variables.
  template <typename GUM_SCALAR>
  void LoopyBeliefPropagation<GUM_SCALAR>::_makeInference() {
    __initStats();
    initApproximationScheme();

    double error = 0.0;

    do {
      //        nextParticle();
      updateApproximationScheme();
      __updateNodeMessage( 0 );
    } while ( continueApproximationScheme( error ) );
    /*
          for ( auto& elt : __sampling_nbr ) {
            elt.second.normalize();
          }*/
  }
} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
