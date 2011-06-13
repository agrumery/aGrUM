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

#include <vector>

// to ease parsing for IDE
#include <agrum/BN/particles/Gibbs.h>

#define DEFAULT_DRAWN 10
#define DEFAULT_BURNIN 3000

namespace gum {

  namespace particle {

    template <typename T_DATA>
    Gibbs<T_DATA>::Gibbs( const AbstractBayesNet<T_DATA>& BN ) :
        __nbr_drawn_by_sample( DEFAULT_DRAWN ),
        __bayesNet(BN) {
      GUM_CONSTRUCTOR( Gibbs );

      // set the correspondance between variables
      const DAG& dag = this->bn().dag();
//    const NodeSet& nodes = dag.nodes();

      for ( DAG::NodeIterator iter = dag.beginNodes(); iter != dag.endNodes(); ++iter ) {
        const DiscreteVariable& var = this->bn().variable( *iter );
        // feed the sample
        __particle << var;
        // feed the __sampling
        Potential<T_DATA>* tmp = new Potential<T_DATA>();
        __sampling_posterior.insert( *iter, tmp );
        ( *tmp ) << var;
        Instantiation* tmp_idx = new Instantiation( *tmp );
        __sampling_idx.insert( *iter, tmp_idx );
        // feed the children
        std::vector<NodeId>* tmp3 = new std::vector<NodeId>();
        const NodeSet& arcs = dag.children( *iter );

        for ( NodeSetIterator iter2 = arcs.begin();
              iter2 != arcs.end(); ++iter2 ) {
          tmp3->push_back( *iter2 );
        }

        __node_children.insert( *iter, tmp3 );

        // feed the instantiation for each cpt
        Instantiation* tmp4 = new Instantiation( this->bn().cpt( *iter ) );
        __cpt_idx.insert( *iter, tmp4 );
      }

    }

    template <typename T_DATA>
    Gibbs<T_DATA>::~Gibbs() {
      GUM_DESTRUCTOR( Gibbs );

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

    template <typename T_DATA> INLINE
    const AbstractBayesNet<T_DATA>& Gibbs<T_DATA>::bn( void) {
			return __bayesNet;
    }
    
    template <typename T_DATA> INLINE
    void Gibbs<T_DATA>::__setValVar( NodeId id, Idx choice ) {
      const DiscreteVariable& v = this->bn().variable( id );
      // the change is directly in __particle in order to use the new
      // drawn values for future draws
      __particle.chgVal( v, choice );
      // propagate the change in each concerned __cpt_idx
      // the first : CPT of node id
      __cpt_idx[id]->chgVal( v, choice );
      // the others : CPTs of its children

      for ( std::vector<NodeId>::iterator iter = __node_children[id]->begin();
            iter != __node_children[id]->end(); iter++ ) {
        __cpt_idx[*iter]->chgVal( v, choice );
      }
    }

    /// how many sample between 2 stop test
    template <typename T_DATA> INLINE
    void Gibbs<T_DATA>::setNbrDrawnBySample( Size s ) {
      __nbr_drawn_by_sample = s;
    }

    /// how many sample between 2 stop test
    template <typename T_DATA> INLINE
    Size Gibbs<T_DATA>::nbrDrawnBySample( ) {
      return __nbr_drawn_by_sample ;
    }

    // give the actual particle
    template <typename T_DATA> inline
    const Instantiation& Gibbs<T_DATA>::particle( void ) {
      return __particle;
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
      T_DATA s = ( T_DATA ) 0;

      for ( I.setFirst(); ! I.end(); I.inc() ) s += proba[I];

      if ( s == ( T_DATA ) 0 ) {
        std::ostringstream str;
        str << "A normalisation factor is 0 in node " << id << " (" << v << ")";

        GUM_ERROR( FatalError, str.str() );
      }

      // draw value
      for ( I.setFirst(); ! I.end(); I.inc() ) {
        if ( proba[I] == ( T_DATA ) 0 ) continue;

        choice = I.val( v );

        T_DATA q = proba[I] / s;

        if ( p <= q ) {
          break;
        }

        p -= q;
      }

      __setValVar( id, choice );
    }

    /// change in __particle a new drawn value for id
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
        tmp = __cpt_idx[id];
        tmp->chgVal( v, current_mod_id );
        posterior.set( posterior_idx, value = this->bn().cpt( id )[*tmp] );

        if ( value == ( T_DATA ) 0 ) {
          continue;
        }

        if ( __evidences.exists( id ) ) {
          posterior.set( posterior_idx, posterior[posterior_idx]* ( value = ( * __evidences[id] )[posterior_idx] ) );

          if ( value == ( T_DATA ) 0 ) {
            continue;
          }
        }

        for ( std::vector<NodeId>::iterator iter = __node_children[id]->begin();
              iter != __node_children[id]->end(); iter++ ) {
          tmp = __cpt_idx[*iter];
          tmp->chgVal( v, current_mod_id );
          //posterior[posterior_idx]*=( value=this->bn().cpt( *iter )[*tmp] );
          posterior.set( posterior_idx, posterior[posterior_idx]* ( value = this->bn().cpt( *iter )[*tmp] ) );

          if ( value == ( T_DATA ) 0 ) {
            continue;
          }
        }
      }

      __drawVar( id );
    }


    /// remove a given evidence from the graph
    template <typename T_DATA> INLINE
    void Gibbs<T_DATA>::eraseEvidence( const Potential<T_DATA>* pot ) {
      const Sequence<const DiscreteVariable *>& vars = pot->variablesSequence();

      if ( vars.size() != 1 )
        GUM_ERROR( SizeError, "The evidence should be one-dimensionnal" );

      NodeId id = this->bn().nodeId( * ( vars.atPos( 0 ) ) );

      if ( __evidences.exists( id ) ) __evidences.erase( id );

      if ( __hard_evidences.exists( id ) ) __hard_evidences.erase( id );
    }

    /// remove all evidence from the graph
    template <typename T_DATA> INLINE
    void Gibbs<T_DATA>::eraseAllEvidence() {
      __evidences.clear();
      __hard_evidences.clear();
    }

    /// insert new evidence in the graph
    template <typename T_DATA>
    void Gibbs<T_DATA>::insertEvidence
    ( const List<const Potential<T_DATA>*>& pot_list ) {
      eraseAllEvidence();

      for ( ListConstIterator<const Potential<T_DATA>*> iter = pot_list.begin();
            iter != pot_list.end(); ++iter ) {
        // check that the evidence is given w.r.t.only one random variable
        const Potential<T_DATA>& pot = **iter;
        const Sequence<const DiscreteVariable *>& vars = pot.variablesSequence();

        if ( vars.size() != 1 )
          GUM_ERROR( SizeError, "The evidence should be one-dimensionnal" );

        Size nb_un = 0;

        Size nb_zero = 0;

        Idx pos_un = 0;

        Instantiation I( pot );

        for ( I.setFirst();! I.end() ; ++I ) {
          if ( pot[I] == ( T_DATA ) 0 ) {
            nb_zero++;
          } else {
            pos_un = I.val( *vars.atPos( 0 ) );
            nb_un++;
          }
        }

        // insert the evidence
        if (( nb_un == 1 ) && ( nb_un + nb_zero == I.domainSize() ) ) {
          __hard_evidences.insert( this->bn().nodeId( * ( vars.atPos( 0 ) ) ), pos_un );
        } else {
          __evidences.insert( this->bn().nodeId( * ( vars.atPos( 0 ) ) ), *iter );
        }
      }
    }

    /// put in __particle a MC sample
    /// This is not a really sample since we take into account evidence without
    /// care about parent of evidence, etc.
    /// This is just a not-so-bad first sample for GIBBS
    template <typename T_DATA>
    void Gibbs<T_DATA>::__MonteCarloSample() {
      // _nodes_array is assumed to be the list of nodes to draw; in a topological-compatible order
      for ( unsigned int it=0;it<__nodes_array.size();it++ ) {
        Idx id = __nodes_array[it];

        const Potential<T_DATA>& cpt = this->bn().cpt( id );

        const DiscreteVariable& v = this->bn().variable( id );

        // we have to build P(x \given instantiation_parent(x)) in proba
        Instantiation I( cpt );

        Sequence<const DiscreteVariable*> seq = I.variablesSequence();

        for ( Sequence<const DiscreteVariable*>::iterator it = seq.begin();
              it != seq.end();++it ) {
          I.chgVal( it.pos() , __particle.val( *it ) );
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


    template <typename T_DATA>
    void Gibbs<T_DATA>::initParticle() {
      initRandom();

      const Sequence<NodeId>& topo = this->bn().getTopologicalOrder();

      __nodes_array.clear();
      // nodes to be drawn : not the ones with hard evidence

      for ( Sequence<NodeId>::iterator iter = topo.begin() ; iter != topo.end();++iter ) {
        if ( ! __hard_evidences.exists( *iter ) ) {
          __nodes_array.push_back( *iter );
        } else {
          // put the right value for instantiated variables
          __setValVar( *iter, __hard_evidences[*iter] );
        }
      }

      __MonteCarloSample();

      if ( __nbr_drawn_by_sample == 0 ) { // means topological ordre for every sample
        __nbr_of_iterations = __nodes_array.size();
      } else { // randomly choosen set of vars of size __nbr_drawn_by_sample
        __nbr_of_iterations = __nbr_drawn_by_sample;

        if ( __nbr_of_iterations > __nodes_array.size() ) __nbr_of_iterations = __nodes_array.size();

        std::random_shuffle( __nodes_array.begin(), __nodes_array.end() );
      }
    }


    template<typename T_DATA>
    void Gibbs<T_DATA>::nextParticle() {
      // GENERATE NEXT SAMPLE
      if ( __nbr_drawn_by_sample == 0 ) {
        // we want to draw all the node always in the same order
        for ( std::vector<NodeId>::iterator it = __nodes_array.begin();
              it != __nodes_array.end();
              ++it ) {
          __GibbsSample( *it );
        }
      } else if ( __nbr_of_iterations == 1 ) { // we want to draw only one randomly chosen node
        __GibbsSample( __nodes_array[std::rand() % __nodes_array.size()] );
      } else { // we want to draw nbr randomly chosen nodes.
        std::vector<NodeId>::iterator it = __nodes_array.begin();

        for ( Size j = 0;
              j < __nbr_of_iterations;
              j++, ++it ) {
          __GibbsSample( *it );
        }

        std::random_shuffle( __nodes_array.begin(), __nodes_array.end() );
      }
    }
  } // namespace particle
} // namespace gum

#endif // DOXYGEN_SHOULD_SKIP_THIS
