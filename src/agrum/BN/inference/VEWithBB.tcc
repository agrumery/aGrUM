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
 * @brief Implementation of lazy propagation for inference Bayesian Networks.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/inference/VEWithBB.h>

namespace gum {

  template <typename GUM_SCALAR>
  INLINE VEWithBB<GUM_SCALAR>::VEWithBB( const IBayesNet<GUM_SCALAR>& bn )
      : BayesNetInference<GUM_SCALAR>( bn )
      , __ve( bn ) {
    GUM_CONSTRUCTOR( VEWithBB );
    __ve.makeInference();
  }

  template <typename GUM_SCALAR>
  INLINE VEWithBB<GUM_SCALAR>::~VEWithBB() {
    GUM_DESTRUCTOR( VEWithBB );
  }
  /*
    template<typename GUM_SCALAR> INLINE
    VEWithBB<GUM_SCALAR>::VEWithBB( const VEWithBB<GUM_SCALAR>& source ) :
    BayesNetInference<GUM_SCALAR>(source) {
      GUM_CONS_CPY( VEWithBB );
      GUM_ERROR( FatalError, "illegal call to VEWithBB copy constructor." );
    }

    template<typename GUM_SCALAR> INLINE
    VEWithBB<GUM_SCALAR>&
    VEWithBB<GUM_SCALAR>::operator=( const VEWithBB& source ) {
      GUM_ERROR( FatalError, "illegal call to VEWithBB copy operator." );
    }*/

  template <typename GUM_SCALAR>
  INLINE void VEWithBB<GUM_SCALAR>::makeInference() {}

  template <typename GUM_SCALAR>
  void VEWithBB<GUM_SCALAR>::insertEvidence(
      const List<const Potential<GUM_SCALAR>*>& pot_list ) {
    for ( const auto& pot : pot_list ) {
      if ( pot->nbrDim() != 1 ) {
        GUM_ERROR( OperationNotAllowed,
                   "Evidence can only be giben w.r.t. one random variable" );
      }

      NodeId varId =
          this->bn().nodeId( *( pot->variablesSequence().atPos( 0 ) ) );
      // size_t count = 0;
      // Instantiation i( *pot );

      // for ( i.setFirst(); ! i.end(); i.inc() ) {
      //  if ( pot->get( i ) == (GUM_SCALAR)1 ) {
      //    ++count;
      //  }

      //  if ( count > 2 ) {
      //    break;
      //  }
      //}

      // if ( count == 1 ) {
      if ( __evidence.exists( varId ) ) {
        __evidence[varId] = pot;
      } else {
        __evidence.insert( varId, pot );
      }
      //}
    }

    __ve.insertEvidence( pot_list );
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void
  VEWithBB<GUM_SCALAR>::eraseEvidence( const Potential<GUM_SCALAR>* e ) {
    if ( e->nbrDim() != 1 ) {
      GUM_ERROR( OperationNotAllowed,
                 "Evidence can only be giben w.r.t. one random variable" );
    }

    __evidence.erase(
        ( this->bn().nodeId( *( e->variablesSequence().atPos( 0 ) ) ) ) );
    __ve.eraseEvidence( e );
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE void VEWithBB<GUM_SCALAR>::eraseAllEvidence() {
    __evidence.clear();
    __ve.eraseAllEvidence();
    this->_invalidatePosteriors();
  }

  template <typename GUM_SCALAR>
  INLINE bool
  VEWithBB<GUM_SCALAR>::__isHardEvidence( const Potential<GUM_SCALAR>* e ) {
    auto sum = 0.0;
    for ( Instantiation i( e ); !i.end(); i.inc(), sum += e->get( i ) ) {
      if ( !( e->get( i ) == 0.0 || e->get( i ) == 1.0 ) ) {
        return false;
      }
    }
    return sum == 1.0;
  }

  template <typename GUM_SCALAR>
  INLINE void
  VEWithBB<GUM_SCALAR>::__fillRequisiteNode( NodeId id,
                                             Set<NodeId>& requisite_nodes ) {
    Set<NodeId> query;
    query.insert( id );
    Set<NodeId> hardEvidence;
    Set<NodeId> softEvidence;

    for ( const auto& elt : __evidence ) {
      if ( __isHardEvidence( elt.second ) ) {
        hardEvidence.insert( elt.first );
      } else {
        softEvidence.insert( elt.first );
      }
    }

    BayesBall bb;
    bb.requisiteNodes(
        this->bn().dag(), query, hardEvidence, softEvidence, requisite_nodes );
  }

  template <typename GUM_SCALAR>
  void VEWithBB<GUM_SCALAR>::__initializeElimination(
      NodeId id,
      UndiGraph& moral_graph,
      NodeProperty<Size>& modalities,
      std::vector<const Potential<GUM_SCALAR>*>& pool ) {
    Set<NodeId> requisite_nodes;
    __fillRequisiteNode( id, requisite_nodes );

    for ( auto node : requisite_nodes ) {

      const auto& pot = this->bn().cpt( node );
      pool.push_back( &pot );

      if ( __evidence.exists( node ) ) {
        pool.push_back( __evidence[node] );
      }

      for ( gum::Size i = 0; i < pot.variablesSequence().size(); ++i ) {

        auto node_i = this->bn().nodeId( *( pot.variablesSequence()[i] ) );
        __addNodeToMoralGraph( node_i, moral_graph, modalities );

        for ( gum::Size j = i + 1; j < pot.variablesSequence().size(); ++j ) {

          auto node_j = this->bn().nodeId( *( pot.variablesSequence()[j] ) );
          __addNodeToMoralGraph( node_j, moral_graph, modalities );

          if ( !moral_graph.existsEdge( node_i, node_j ) ) {
            moral_graph.addEdge( node_i, node_j );
          }
        }
      }
    }
  }

  template <typename GUM_SCALAR>
  void VEWithBB<GUM_SCALAR>::__addNodeToMoralGraph(
      NodeId node, UndiGraph& moral_graph, NodeProperty<Size>& modalities ) {
    if ( !moral_graph.exists( node ) ) {
      moral_graph.addNode( node );
      modalities.insert( node, this->bn().variable( node ).domainSize() );
    }
  }

  template <typename GUM_SCALAR>
  void
  VEWithBB<GUM_SCALAR>::_fillPosterior( NodeId id,
                                        Potential<GUM_SCALAR>& posterior ) {
    UndiGraph moral_graph;
    NodeProperty<Size> modalities;
    std::vector<const Potential<GUM_SCALAR>*> pool;
    Set<Potential<GUM_SCALAR>*> trash;
    __initializeElimination( id, moral_graph, modalities, pool );

    DefaultTriangulation triang( &moral_graph, &modalities );
    const auto& elim_order = triang.eliminationOrder();

    for ( auto node : elim_order ) {
      if ( node != id ) {
        __eliminateNode( node, pool, trash );
      }
    }

    while ( pool.size() > 1 ) {
      const auto& p1 = *( pool.back() );
      pool.pop_back();
      const auto& p2 = *( pool.back() );
      pool.pop_back();
      auto mult = new Potential<GUM_SCALAR>( p1 * p2 );
      pool.push_back( mult );
      trash.insert( mult );
    }

    // Copy result in marginal
    posterior = *( pool.back() );
    posterior.normalize();

    // Cleaning up the mess
    for ( auto pot : trash ) {
      delete pot;
    }
  }

  template <typename GUM_SCALAR>
  void VEWithBB<GUM_SCALAR>::__eliminateNode(
      NodeId id,
      std::vector<const Potential<GUM_SCALAR>*>& pool,
      Set<Potential<GUM_SCALAR>*>& trash ) {
    auto var = &( this->bn().variable( id ) );

    Potential<GUM_SCALAR> result;
    Set<const DiscreteVariable*> var_set;
    var_set.insert( var );
    Set<const Potential<GUM_SCALAR>*> factors;

    std::vector<const Potential<GUM_SCALAR>*> tmp;
    for ( auto p : pool ) {
      if ( p->contains( *var ) ) {
        factors.insert( p );
      } else {
        tmp.push_back( p );
      }
    }
    pool.swap( tmp );

    if ( factors.size() == 0 ) {
      return;
    } else if ( factors.size() == 1 ) {
      result = ( *factors.begin() )->margSumOut( var_set );
    } else {
      MultiDimCombinationDefault<GUM_SCALAR, Potential> Comb( [](
          const Potential<GUM_SCALAR>& t1, const Potential<GUM_SCALAR>& t2 ) {
        return new Potential<GUM_SCALAR>( t1 * t2 );
      } );
      auto tmp = Comb.combine( factors );
      result = tmp->margSumOut( var_set );
      delete tmp;
    }

    for ( auto const_p : factors ) {
      auto p = const_cast<Potential<GUM_SCALAR>*>( const_p );
      if ( trash.exists( p ) ) {
        trash.erase( p );
        delete p;
      }
    }

    auto pot = new Potential<GUM_SCALAR>( result );
    pool.push_back( pot );
    trash.insert( pot );
  }

} /* namespace gum */
