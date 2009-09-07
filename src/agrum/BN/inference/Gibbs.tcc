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

#define DEFAULT_DRAWN 10
#define DEFAULT_BURNIN 3000
#define DEFAULT_MAXITER 10000000
#define DEFAULT_EPSILON 1e-4*log(2)
#define DEFAULT_MIN_EPSILON_RATE 5e-4
#define DEFAULT_PERIODE_SIZE 500
#define DEFAULT_VERBOSITY true

// to ease parsing for IDE
#include <agrum/BN/inference/Gibbs.h>

namespace gum {
  /// default constructor
  template <typename T_DATA>
  Gibbs<T_DATA>::Gibbs( const BayesNet<T_DATA>& BN ) :
      BayesNetInference<T_DATA>( BN ),
      __eps( DEFAULT_EPSILON ),
      __min_rate_eps( DEFAULT_MIN_EPSILON_RATE ),
      __nbr_drawn_by_sample( DEFAULT_DRAWN ),
      __burn_in( DEFAULT_BURNIN ),
      __max_iter( DEFAULT_MAXITER ),
      __periode_size( DEFAULT_PERIODE_SIZE ),
      __verbosity( DEFAULT_VERBOSITY ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Gibbs );
    // set the correspondance between variables
    const DAG& dag = this->bn().dag();
//    const NodeSet& nodes = dag.nodes();

    for ( DAG::NodeIterator iter = dag.beginNodes(); iter != dag.endNodes(); ++iter ) {
      const DiscreteVariable& var = this->bn().variable( *iter );
      // feed the sample
      __current_sample << var;
      // feed the __sampling
      Potential<T_DATA>* tmp = new Potential<T_DATA>();
      ( *tmp ) << var;
      Instantiation* tmp_idx = new Instantiation( *tmp );
      __sampling_nbr.insert( *iter, tmp );
      __sampling_idx.insert( *iter, tmp_idx );
      // feed the posterior
      Potential<T_DATA>* tmp2 = new Potential<T_DATA>();
      ( *tmp2 ) << var;
      __sampling_posterior.insert( *iter, tmp2 );
      // feed the children
      std::vector<NodeId>* tmp3 = new std::vector<NodeId>();
      const ArcSet& arcs = dag.children( *iter );

      for ( ArcSetIterator iter2 = arcs.begin();
            iter2 != arcs.end(); ++iter2 ) {
        NodeId idChild = ( *iter2 ).other( *iter ) ;
        tmp3->push_back( idChild );
      }

      __node_children.insert( *iter, tmp3 );

      // feed the instantiation for each cpt
      Instantiation* tmp4 = new Instantiation( this->bn().cpt( *iter ) );
      __cpt_idx.insert( *iter, tmp4 );
    }

    setRequiredInference();
  }

  /// destructor
  template <typename T_DATA> INLINE
  Gibbs<T_DATA>::~Gibbs() {
    // for debugging purposes
    GUM_DESTRUCTOR( Gibbs );
    // remove all the created potentials and instantiations

    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter )
      delete( *iter );

    for ( HashTableIterator<NodeId, Instantiation*> iter = __sampling_idx.begin();
          iter != __sampling_idx.end(); ++iter )
      delete( *iter );

    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_posterior.begin();
          iter != __sampling_posterior.end(); ++iter )
      delete( *iter );

    for ( HashTableIterator<NodeId, std::vector<NodeId>*> iter =
            __node_children.begin();
          iter != __node_children.end(); ++iter )
      delete( *iter );

    for ( HashTableIterator<NodeId, Instantiation*> iter = __cpt_idx.begin();
          iter != __cpt_idx.end(); ++iter )
      delete( *iter );
  }

  /// stopping criterion on KL(t,t+1)
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setEpsilon( double d ) {
    __eps = d;
  }

  /// stopping criterion on dKL(t,t+1)/dt
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setMinEpsilonRate( double d ) {
    __min_rate_eps = d;
  }

  /// stopping criterion on number of iteration
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setMaxIter( Size s ) {
    __max_iter = s;
  }

  /// size of burn in on number of iteration
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setBurnIn( Size s ) {
    __burn_in = s;
  }

  /// 0 for deterministic order, n for n vars randomly choosen
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setPeriodeSize( Size p ) {
    __periode_size = p;
  }

  /// how many sample between 2 stop test
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setNbrDrawnBySample( Size s ) {
    __nbr_drawn_by_sample = s;
  }

  /// verbosity
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setVerbosity( bool v ) {
    __verbosity = v;
  }

  /// setter/getter for __inference_is_required
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::setRequiredInference() {
    this->_invalidateMarginals();
    __inference_is_required = true;
  }

  /// setter/getter for __inference_is_required
  template <typename T_DATA> INLINE
  bool Gibbs<T_DATA>::isInferenceRequired() {
    return __inference_is_required;
  }

  /// setter/getter for __inference_is_required
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::__unsetRequiredInference() {
    __inference_is_required = false;
  }

  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::__initStats() {
    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter ) {
      ( *iter )->fill(( T_DATA )0 );
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
  double Gibbs<T_DATA>::__updateStats_with_err( Size nb ) {
    Size nbr = nb + 1; // we compute the new iteration
    double sum_entropy = 0;

    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter ) {
      NodeId id = iter.key();
      const DiscreteVariable& v = this->bn().variable( id );
      __sampling_idx[id]->chgVal( v, __current_sample.val( v ) );

      T_DATA n_v = ( *iter )->get( *__sampling_idx[id] ) + 1;
      ( *iter )->set( *__sampling_idx[id], n_v );

      if ( n_v == 1 ) sum_entropy += 100;
      else sum_entropy += n_v * log( n_v / ( n_v - 1 ) );
    }

    return sum_entropy / nbr + __sampling_nbr.size()*log(( double )nbr / nb );
  }

  /** same as __updateStats_with_err but with no entropy computation */
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::__updateStats_without_err() {
    for ( HashTableIterator<NodeId, Potential<T_DATA>*> iter =
            __sampling_nbr.begin();
          iter != __sampling_nbr.end(); ++iter ) {
      NodeId id = iter.key();
      const DiscreteVariable& v = this->bn().variable( id );
      __sampling_idx[id]->chgVal( v, __current_sample.val( v ) );
      //( *( *iter ) )[*__sampling_idx[id]]+=( T_DATA )1;
      ( *iter )->set( *__sampling_idx[id], ( *iter )->get( *__sampling_idx[id] ) + 1 );
    }
  }

  /// remove a given evidence from the graph
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::eraseEvidence( const Potential<T_DATA>* pot ) {
    const Sequence<const DiscreteVariable *>& vars = pot->variablesSequence();

    if ( vars.size() != 1 )
      GUM_ERROR( SizeError, "The evidence should be one-dimensionnal" );

    NodeId id = this->bn().nodeId( *( vars.atPos( 0 ) ) );

    if ( __evidences.exists( id ) ) __evidences.erase( id );

    if ( __hard_evidences.exists( id ) ) __hard_evidences.erase( id );

    setRequiredInference( );
  }

  /// remove all evidence from the graph
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::eraseAllEvidence() {
    __evidences.clear();
    __hard_evidences.clear();
    setRequiredInference();
  }

  /// insert new evidence in the graph
  template <typename T_DATA>
  void Gibbs<T_DATA>::insertEvidence
  ( const List<const Potential<T_DATA>*>& pot_list ) {
    this->_invalidateMarginals();
    __evidences.clear();
    __hard_evidences.clear();

    for ( ListConstIterator<const Potential<T_DATA>*> iter = pot_list.begin();
          iter != pot_list.end(); ++iter ) {
      // check that the evidence is given w.r.t.only one random variable
      const Potential<T_DATA>& pot = **iter;
      const Sequence<const DiscreteVariable *>& vars = pot.variablesSequence();

      if ( vars.size() != 1 )
        GUM_ERROR( SizeError, "The evidence should be one-dimensionnal" );

      Size nb_un = 0;Size nb_zero = 0;Idx pos_un = 0;Instantiation I( pot );

      for ( I.setFirst();! I.end() ; ++I ) {
        if ( pot[I] == ( T_DATA )0 ) {
          nb_zero++;
        } else {
          pos_un = I.val( *vars.atPos( 0 ) );
          nb_un++;
        }
      }

      // insert the evidence
      if (( nb_un == 1 ) && ( nb_un + nb_zero == I.domainSize() ) ) {
        __hard_evidences.insert( this->bn().nodeId( *( vars.atPos( 0 ) ) ), pos_un );
      } else {
        __evidences.insert( this->bn().nodeId( *( vars.atPos( 0 ) ) ), *iter );
      }
    }

    setRequiredInference( );
  }

  /// Returns the probability of the variable.
  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::_fillMarginal( NodeId id, Potential<T_DATA>& marginal ) {
    if ( isInferenceRequired() ) makeInference();

    marginal = *( __sampling_nbr[id] );

    marginal.normalize();
  }

  template <typename T_DATA> INLINE
  Idx Gibbs<T_DATA>::__getValVar( NodeId id ) {
    return __current_sample.val( this->bn().variable( id ) );
  }

  template <typename T_DATA> INLINE
  void Gibbs<T_DATA>::__chgValVar( NodeId id, Idx choice ) {
    const DiscreteVariable& v = this->bn().variable( id );
    // the change is directly in __current_sample in order to use the new
    // drawn values for future draws
    __current_sample.chgVal( v, choice );
    // propagate the change in each concerned __cpt_idx
    // the first : CPT of node id
    __cpt_idx[id]->chgVal( v, choice );
    // the others : CPTs of its children

    for ( std::vector<NodeId>::iterator iter = __node_children[id]->begin();
          iter != __node_children[id]->end(); iter++ ) {
      __cpt_idx[*iter]->chgVal( v, choice );
    }
  }

  /// put in __next_sample a value for variable id,
  /// @warning : proba is a probability for variable id
  template <typename T_DATA>
  void Gibbs<T_DATA>::__drawVar( NodeId id ) {
    const DiscreteVariable& v = this->bn().variable( id );
    Potential<T_DATA>& proba = *__sampling_posterior[id];
    T_DATA p = ( T_DATA ) getRandomProba();
    // use of __sampling_idx for shrink the number of temporary Instantiation
    Instantiation& I = *__sampling_idx[id];
    Idx choice = 0;
    // normalisation
    T_DATA s = ( T_DATA )0;

    for ( I.setFirst(); ! I.end(); I.inc( ) ) s += proba[I];

    if ( s == ( T_DATA )0 ) {
      std::ostringstream str;
      str << "A normalisation factor is 0 in node " << id << " (" << v << ")";

      GUM_ERROR( FatalError, str.str() );
    }

    // draw value
    for ( I.setFirst(); ! I.end(); I.inc( ) ) {
      if ( proba[I] == ( T_DATA )0 ) continue;

      choice = I.val( v );

      T_DATA q = proba[I] / s;

      if ( p <= q ) {
        break;
      }

      p -= q;
    }

    __chgValVar( id, choice );
  }

  /// put in __current_sample a MC sample
  /// This is not a really sample since we take into account evidence without
  /// care about parent of evidence, etc.
  /// This is just a not-so-bad first sample for GIBBS
  template <typename T_DATA>
  void Gibbs<T_DATA>::__MonteCarloSample() {
    const Sequence<NodeId>& topo = this->bn().getTopologicalOrder();

    for ( Sequence<NodeId>::iterator iter = topo.begin() ; iter != topo.end();++iter ) {
      Idx id = *iter;
      // we do not draw instantiated variables

      if ( __hard_evidences.exists( id ) ) continue;

      const Potential<T_DATA>& cpt = this->bn().cpt( id );

      const DiscreteVariable& v = this->bn().variable( id );

      // we have to build P(x \given instantiation_parent(x)) in proba
      Instantiation I( cpt );

      Sequence<const DiscreteVariable*> seq = I.variablesSequence();

      for ( Sequence<const DiscreteVariable*>::iterator it = seq.begin();
            it != seq.end();++it ) {
        I.chgVal( it.pos() , __current_sample.val( *it ) );
      }

      Potential<T_DATA>& proba = *__sampling_posterior[id];

      for ( I.setFirstVar( v ); ! I.end(); I.incVar( v ) ) {
        if ( __evidences.exists( id ) ) {
          proba.set( I, ( * __evidences[id] )[I] );
        } else {
          proba.set( I, cpt[I] );
        }
      }

      __drawVar( id );
    }
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

  /// change in __current_sample a new drawn value for id
  template<typename T_DATA>
  void Gibbs<T_DATA>::__GibbsSample( NodeId id ) {
    const DiscreteVariable& v = this->bn().variable( id );
    // we have to build P(x \given instantiation_markovblanket(x)) in posterior
    // see Pearl'88, 4.71 p218
    Potential<T_DATA>& posterior = *__sampling_posterior[id];
    Instantiation& posterior_idx = *__sampling_idx[id];

    Instantiation* tmp;
    T_DATA value;

    for ( posterior_idx.setFirst();! posterior_idx.end();posterior_idx.inc() ) {
      Idx current_mod_id = posterior_idx.val( v );
      tmp = __cpt_idx[id];tmp->chgVal( v, current_mod_id );
      posterior.set( posterior_idx, value = this->bn().cpt( id )[*tmp] );

      if ( value == ( T_DATA )0 ) {
        continue;
      }

      if ( __evidences.exists( id ) ) {
        posterior.set( posterior_idx, posterior[posterior_idx]*( value = ( * __evidences[id] )[posterior_idx] ) );

        if ( value == ( T_DATA )0 ) {
          continue;
        }
      }

      for ( std::vector<NodeId>::iterator iter = __node_children[id]->begin();
            iter != __node_children[id]->end(); iter++ ) {
        tmp = __cpt_idx[*iter];tmp->chgVal( v, current_mod_id );
        //posterior[posterior_idx]*=( value=this->bn().cpt( *iter )[*tmp] );
        posterior.set( posterior_idx, posterior[posterior_idx]*( value = this->bn().cpt( *iter )[*tmp] ) );

        if ( value == ( T_DATA )0 ) {
          continue;
        }
      }
    }

    __drawVar( id );
  }

  template<typename T_DATA>
  void Gibbs<T_DATA>::__generateSample( std::vector<NodeId>& nodes_array,
                                        Size nbr ) {
    // GENERATE NEXT SAMPLE
    if ( __nbr_drawn_by_sample == 0 ) {
      // we want to draw all the node always in the same order
      for ( std::vector<NodeId>::iterator it = nodes_array.begin();
            it != nodes_array.end();
            ++it ) {
        __GibbsSample( *it );
      }
    } else if ( nbr == 1 ) { // we want to draw only one randomly chosen node
      __GibbsSample( nodes_array[std::rand() % nodes_array.size()] );
    } else { // we want to draw nbr randomly chosen nodes.
      std::vector<NodeId>::iterator it = nodes_array.begin();

      for ( Size j = 0;
            j < nbr;
            j++, ++it ) {
        __GibbsSample( *it );
      }

      std::random_shuffle( nodes_array.begin(), nodes_array.end() );
    }
  }

  /// Returns the probability of the variables.
  template <typename T_DATA>
  void Gibbs<T_DATA>::makeInference( ) {
    if ( ! isInferenceRequired() ) return;

    initRandom();

    // build a std::vector of id
    std::vector<NodeId> nodes_array;

    const Sequence<NodeId>& topo = this->bn().getTopologicalOrder();

    // nodes to be drawn : not the ones with hard evidence
    for ( Sequence<NodeId>::iterator iter = topo.begin() ; iter != topo.end();++iter ) {
      if ( ! __hard_evidences.exists( *iter ) ) {
        nodes_array.push_back( *iter );
      } else {
        // put the right value for instantiated variables
        __chgValVar( *iter, __hard_evidences[*iter] );
      }
    }


    /// nbr of iteration by sampling
    Size nbr;

    if ( __nbr_drawn_by_sample == 0 ) { // means topological ordre for every sample
      nbr = nodes_array.size();
    } else { // randomly choosen set of vars of size __nbr_drawn_by_sample
      nbr = __nbr_drawn_by_sample;

      if ( nbr > nodes_array.size() ) nbr = nodes_array.size();

      std::random_shuffle( nodes_array.begin(), nodes_array.end() );
    }

    __initStats();

    __MonteCarloSample();

    // BURN IN

    for ( Idx i = 0;i < __burn_in;i++ ) __generateSample( nodes_array, nbr );

    double err = 10.0, last_err = 0.0;

    double err_rate = 10.0;

    Size nb_iter;

    // SAMPLING
    for ( nb_iter = 1;nb_iter <= __max_iter;nb_iter++ ) {
      __generateSample( nodes_array, nbr );

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

    if ( __verbosity ) GUM_TRACE_VAR( nb_iter );

    __unsetRequiredInference();
  }


} /* namespace gum */


#endif    // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
