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
 * @brief Implementation of Gibbs sampling for inference in Bayesian Networks.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <sstream>
#include <string>
#include <algorithm>

#define DEFAULT_MAXITER 10000000
#define DEFAULT_EPSILON 1e-4*log(2)
#define DEFAULT_MIN_EPSILON_RATE 1e-4
#define DEFAULT_PERIODE_SIZE 500
#define DEFAULT_VERBOSITY false

// to ease parsing for IDE
#include <agrum/BN/inference/GibbsInference.h>

namespace gum {
  /// default constructor
  template <typename T_DATA>
  GibbsInference<T_DATA>::GibbsInference( const AbstractBayesNet<T_DATA>& BN ) :
      BayesNetInference <T_DATA> ( BN ),
      particle::Gibbs<T_DATA> ( BN ),
      __eps( DEFAULT_EPSILON ),
      __min_rate_eps( DEFAULT_MIN_EPSILON_RATE ),
      __max_iter( DEFAULT_MAXITER ),
      __periode_size( DEFAULT_PERIODE_SIZE ),
      __burn_in( DEFAULT_BURNIN ),
      __verbosity( DEFAULT_VERBOSITY ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( GibbsInference );

    // set the correspondance between variables
    const DAG& dag = bn().dag();
//    const NodeSet& nodes = dag.nodes();

    for ( DAG::NodeIterator iter = dag.beginNodes(); iter != dag.endNodes(); ++iter ) {
      const DiscreteVariable& var = bn().variable( *iter );
      // feed the __sampling
      Potential<T_DATA>* tmp = new Potential<T_DATA>();
      ( *tmp ) << var;
      __sampling_nbr.insert( *iter, tmp );
    }

    setRequiredInference();
  }

  /// destructor
  template <typename T_DATA> INLINE
  GibbsInference<T_DATA>::~GibbsInference() {
    // for debugging purposes
    GUM_DESTRUCTOR( Gibbs );
    // remove all the created potentials and instantiations

    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter )
      delete( *iter );
  }

  /// stopping criterion on KL(t,t+1)
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setEpsilon( double d ) {
    __eps = d;
  }

  /// stopping criterion on dKL(t,t+1)/dt
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setMinEpsilonRate( double d ) {
    __min_rate_eps = d;
  }

  /// stopping criterion on number of iteration
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setMaxIter( Size s ) {
    __max_iter = s;
  }

  /// size of burn in on number of iteration
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setBurnIn( Size s ) {
    __burn_in = s;
  }

  /// 0 for deterministic order, n for n vars randomly choosen
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setPeriodeSize( Size p ) {
    __periode_size = p;
  }

  /// verbosity
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setVerbosity( bool v ) {
    __verbosity = v;
  }

  /// setter/getter for __inference_is_required
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::setRequiredInference() {
    this->_invalidateMarginals();
    __inference_is_required = true;
  }

  /// setter/getter for __inference_is_required
  template <typename T_DATA> INLINE
  bool GibbsInference<T_DATA>::isInferenceRequired() {
    return __inference_is_required;
  }

  /// setter/getter for __inference_is_required
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::__unsetRequiredInference() {
    __inference_is_required = false;
  }

  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::__initStats() {
    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter ) {
      ( *iter )->fill(( T_DATA ) 0 );
    }
  }

  /**
     As a stopping criterion, we use sum of entropy.
     To compute entropy,
     instead of \sum p ln \frac{p}{q}
     We use the fact that we have Frequency : P and Q on a (nbr=nb+1) data set.
     After simplification, it remains that
     entropy=\frac{1}{nbr}ln P_i \frac{P_i}{Q_i}+ln{nbr-1}{nbr}
  */
  template <typename T_DATA> INLINE
  double GibbsInference<T_DATA>::__updateStats_with_err( Size nb ) {
    Size nbr = nb + 1; // we compute the new iteration
    double sum_entropy = 0;

    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter ) {
      //NodeId id = iter.key();
      //const DiscreteVariable& v = bn().variable( id );
      //__sampling_idx[id]->chgVal( v, __current_sample.val( v ) );
      //T_DATA n_v = ( *iter )->get( *__sampling_idx[id] ) + 1;
      //( *iter )->set( *__sampling_idx[id], n_v );

      T_DATA n_v=1+(*iter)->get(particle());
      (*iter)->set(particle(),n_v);
    
      if ( n_v == (T_DATA)1 ) sum_entropy += 100;
      else sum_entropy += n_v * log( n_v / ( n_v - 1 ) );
    }

    return sum_entropy / nbr + __sampling_nbr.size() *log(( double ) nbr / nb );
  }

  /** same as __updateStats_with_err but with no entropy computation */
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::__updateStats_without_err() {
    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter ) {
      //NodeId id = iter.key();
      //const DiscreteVariable& v = bn().variable( id );
      //__sampling_idx[id]->chgVal( v, __current_sample.val( v ) );
      //( *iter )->set( *__sampling_idx[id], ( *iter )->get( *__sampling_idx[id] ) + 1 );
      (*iter)->set(particle(),(*iter)->get(particle())+1);
    }
  }

  /// remove a given evidence from the graph
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::eraseEvidence( const Potential<T_DATA>* pot ) {
    particle::Gibbs<T_DATA>::eraseEvidence( pot );
    setRequiredInference();
  }

  /// remove all evidence from the graph
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::eraseAllEvidence() {
    particle::Gibbs<T_DATA>::eraseAllEvidence();
    setRequiredInference();
  }

  /// insert new evidence in the graph
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::insertEvidence
  ( const List<const Potential<T_DATA>*>& pot_list ) {
    this->_invalidateMarginals();
    particle::Gibbs<T_DATA>::insertEvidence( pot_list );
    setRequiredInference();
  }

  /// Returns the probability of the variable.
  template <typename T_DATA> INLINE
  void GibbsInference<T_DATA>::_fillMarginal( NodeId id, Potential<T_DATA>& marginal ) {
    if ( isInferenceRequired() ) makeInference();

    marginal = * ( __sampling_nbr[id] );

    marginal.normalize();
  }

  INLINE
  void add_and_instancie( Instantiation& I, const DiscreteVariable& v ,
                          const Instantiation& __current_sample ) {
    try {
      I << v;
      I.chgVal( v, __current_sample.val( v ) );
    } catch ( DuplicateElement e ) {
      // do nothing, it's OK
    }
  }



  /// Returns the probability of the variables.
  template <typename T_DATA>
  void GibbsInference<T_DATA>::makeInference() {
    if ( ! isInferenceRequired() ) return;

    initParticle();

    // BURN IN
    for ( Idx i = 0;i < __burn_in;i++ ) nextParticle( );

    __initStats();

    double err = 10.0, last_err = 0.0, err_rate = 10.0;

    Size nb_iter;

    // SAMPLING

    for ( nb_iter = 1;nb_iter <= __max_iter;nb_iter++ ) {
      nextParticle( );

      if ( nb_iter % __periode_size == 0 ) {
        last_err = err;

        if (( err = __updateStats_with_err( nb_iter + __burn_in ) ) < __eps ) {
          if ( __verbosity ) GUM_TRACE( "Stop by err=" << err );

          break;
        }

        if (( err_rate = fabs(( last_err - err ) / err ) ) < __min_rate_eps ) {
          if ( __verbosity ) GUM_TRACE( "Stop by err_rate=" << err_rate );

          break;
        }
      } else {
        __updateStats_without_err();
      }
    }

    if ( __verbosity ) {
      if ( nb_iter>__max_iter ) {
        GUM_TRACE( "Stop by max_iteration="<<__max_iter );
      } else {
        GUM_TRACE( "#iterations : "<<nb_iter );
      }
    }

    __unsetRequiredInference();
  }


} /* namespace gum */


#endif    // DOXYGEN_SHOULD_SKIP_THIS
