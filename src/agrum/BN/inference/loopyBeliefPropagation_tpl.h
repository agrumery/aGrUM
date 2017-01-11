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

#define LBP_DEFAULT_MAXITER 100
#define LBP_DEFAULT_EPSILON 1e-8
#define LBP_DEFAULT_MIN_EPSILON_RATE 1e-10
#define LBP_DEFAULT_PERIOD_SIZE 1
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
    setBurnIn(0); //no burn in for LBP

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
      }
    }
  }

  template <typename GUM_SCALAR>
  void LoopyBeliefPropagation<GUM_SCALAR>::_updateOutdatedBNStructure() {
    __init_messages();
  }


  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdPi( NodeId X ) {
    const auto& varX = this->BN().variable( X );

    auto piX = this->BN().cpt( X );
    for ( const auto& U : this->BN().dag().parents( X ) ) {
      piX *= __messages[Arc( U, X )];
    }
    piX = piX.margSumIn( {&varX} );

    return piX;
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdPi( NodeId X, NodeId except ) {
    const auto& varX = this->BN().variable( X );
    const auto& varExcept = this->BN().variable( except );
    auto        piXexcept = this->BN().cpt( X );
    for ( const auto& U : this->BN().dag().parents( X ) ) {
      if ( U != except ) {
        piXexcept *= __messages[Arc( U, X )];
      }
    }
    piXexcept = piXexcept.margSumIn( {&varX, &varExcept} );
    return piXexcept;
  }


  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdLambda( NodeId X ) {
    Potential<GUM_SCALAR> lamX;
    if ( this->hasEvidence( X ) ) {
      lamX = *( this->evidence()[X] );
    } else {
      lamX.add( this->BN().variable( X ) );
      lamX.fill( 1 );
    }
    for ( const auto& Y : this->BN().dag().children( X ) ) {
      lamX *= __messages[Arc( Y, X )];
    }

    return lamX;
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  LoopyBeliefPropagation<GUM_SCALAR>::__computeProdLambda( NodeId X,
                                                           NodeId except ) {
    Potential<GUM_SCALAR> lamXexcept;
    if ( this->hasEvidence( X ) ) {  //
      lamXexcept = *this->evidence()[X];
    } else {
      lamXexcept.add( this->BN().variable( X ) );
      lamXexcept.fill( 1 );
    }
    for ( const auto& Y : this->BN().dag().children( X ) ) {
      if ( Y != except ) {
        lamXexcept *= __messages[Arc( Y, X )];
      }
    }

    return lamXexcept;
  }


  template <typename GUM_SCALAR>
  GUM_SCALAR LoopyBeliefPropagation<GUM_SCALAR>::__updateNodeMessage( NodeId X ) {
    auto piX = __computeProdPi( X );
    auto lamX = __computeProdLambda( X );

    GUM_SCALAR KL = 0;
    Arc        argKL( 0, 0 );

    // update lambda_par (for arc U->x)
    for ( const auto& U : this->BN().dag().parents( X ) ) {
      auto newLambda = ( __computeProdPi( X, U ) * lamX )
                           .margSumIn( {&this->BN().variable( U )} );
      newLambda.normalize();
      GUM_SCALAR ekl = static_cast<GUM_SCALAR>( 0 );
      try {
        ekl = __messages[Arc( X, U )].KL( newLambda );
      } catch ( InvalidArgument ) {
        GUM_ERROR( InvalidArgument, "Not compatible pi during computation" );
      } catch ( FatalError ) {  // 0 misplaced
        ekl = std::numeric_limits<GUM_SCALAR>::infinity();
      }
      if ( ekl > KL ) {
        KL = ekl;
        argKL = Arc( X, U );
      }
      __messages.set( Arc( X, U ), newLambda );
    }

    // update pi_child (for arc x->child)
    for ( const auto& Y : this->BN().dag().children( X ) ) {
      auto newPi = ( piX * __computeProdLambda( X, Y ) );
      newPi.normalize();
      GUM_SCALAR ekl = KL;
      try {
        ekl = __messages[Arc( X, Y )].KL( newPi );
      } catch ( InvalidArgument ) {
        GUM_ERROR( InvalidArgument, "Not compatible pi during computation" );
      } catch ( FatalError ) {  // 0 misplaced
        ekl = std::numeric_limits<GUM_SCALAR>::infinity();
      }
      if ( ekl > KL ) {
        KL = ekl;
        argKL = Arc( X, Y );
      }
      __messages.set( Arc( X, Y ), newPi );
    }

    return KL;
  }

  template <typename GUM_SCALAR>
  INLINE void LoopyBeliefPropagation<GUM_SCALAR>::__initStats() {
    __init_messages();
    for ( const auto& node : this->BN().topologicalOrder() ) {
      __updateNodeMessage( node );
    }
  }


  /// Returns the probability of the variables.
  template <typename GUM_SCALAR>
  void LoopyBeliefPropagation<GUM_SCALAR>::_makeInference() {
    __initStats();
    initApproximationScheme();

    std::vector<NodeId> shuffleIds;
    for ( const auto& node : this->BN().nodes() )
      shuffleIds.push_back( node );

    auto engine = std::default_random_engine{};

    GUM_SCALAR error = 0.0;
    do {
      std::shuffle( std::begin( shuffleIds ), std::end( shuffleIds ), engine );
      updateApproximationScheme();
      for ( const auto& node : shuffleIds ) {
        GUM_SCALAR e = __updateNodeMessage( node );
        if ( e > error ) error = e;
      }
    } while ( continueApproximationScheme( error ) );
  }


  /// Returns the probability of the variable.
  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  LoopyBeliefPropagation<GUM_SCALAR>::_posterior( NodeId id ) {
    auto p = __computeProdPi( id ) * __computeProdLambda( id );
    p.normalize();
    __posteriors.set( id, p );

    return __posteriors[id];
  }
} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
