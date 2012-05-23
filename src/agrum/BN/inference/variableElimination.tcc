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
 * @author Lionel Torti
 */
// ============================================================================
#include <agrum/BN/inference/variableElimination.h>
// ============================================================================
namespace gum {

  template<typename T_DATA> INLINE
  VariableElimination<T_DATA>::VariableElimination( const AbstractBayesNet<T_DATA>& bn ):
    BayesNetInference<T_DATA>( bn ) {
    GUM_CONSTRUCTOR( VariableElimination );
  }

  template<typename T_DATA> INLINE
  VariableElimination<T_DATA>::~VariableElimination() {
    GUM_DESTRUCTOR( VariableElimination );
  }

  template<typename T_DATA> INLINE
  VariableElimination<T_DATA>::VariableElimination( const VariableElimination<T_DATA>& source ) {
    GUM_CONS_CPY( VariableElimination );
    GUM_ERROR( FatalError, "illegal call to VariableElimination copy constructor." );
  }

  template<typename T_DATA> INLINE
  VariableElimination<T_DATA>&
  VariableElimination<T_DATA>::operator=( const VariableElimination& source ) {
    GUM_ERROR( FatalError, "illegal call to VariableElimination copy operator." );
  }

  template<typename T_DATA> INLINE
  const std::vector<NodeId>&
  VariableElimination<T_DATA>::eliminationOrder() const {
    if( __eliminationOrder.size() != 0 ) {
      return __eliminationOrder;
    } else {
      GUM_ERROR( OperationNotAllowed, "Elimination order uninitialized." );
    }
  }

  template<typename T_DATA> INLINE
  void
  VariableElimination<T_DATA>::setEliminiationOrder( const std::vector<NodeId>& elim ) {
    __eliminationOrder = elim;
  }

  template<typename T_DATA> INLINE
  void
  VariableElimination<T_DATA>::makeInference() {
    __computeEliminationOrder();
    __createInitialPool();
  }

  template<typename T_DATA>
  void
  VariableElimination<T_DATA>::insertEvidence( const List<const Potential<T_DATA>*>& pot_list ) {
    for( ListConstIterator< const Potential<T_DATA>* > iter = pot_list.begin(); iter != pot_list.end(); ++iter ) {
      if( ( *iter )->nbrDim() != 1 ) {
        GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
      }

      NodeId varId = this->bn().nodeId( ( *iter )->variable( 0 ) );

      try {
        __evidences[varId] = *iter;
      } catch( NotFound& ) {
        __evidences.insert( varId, *iter );
      }
    }

    this->_invalidateMarginals();
  }

  template<typename T_DATA> INLINE
  void
  VariableElimination<T_DATA>::eraseEvidence( const Potential<T_DATA>* e ) {
    if( e->nbrDim() != 1 ) {
      GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
    }

    __evidences.erase( this->bn().nodeId( e->variable( 0 ) ) );
    this->_invalidateMarginals();
  }

  template<typename T_DATA> INLINE
  void
  VariableElimination<T_DATA>::eraseAllEvidence() {
    __evidences.clear();
    this->_invalidateMarginals();
  }

  template <typename T_DATA>
  void
  VariableElimination<T_DATA>::eliminateNodes( const std::vector<NodeId>& elim_order,
      Set< Potential<T_DATA>* >& pool,
      Set< Potential<T_DATA>* >& trash ) {
    for( size_t i = 0; i < elim_order.size(); ++i ) {
      __eliminateNode( elim_order[i], pool, trash );
    }
  }

  template<typename T_DATA>
  void
  VariableElimination<T_DATA>::_fillMarginal( NodeId id, Potential<T_DATA>& marginal ) {
    __computeEliminationOrder();
    __createInitialPool();
    Set< Potential<T_DATA>* > pool( __pool );

    // for ( typename Property< const Potential<T_DATA>* >::onNodes::iterator iter = __evidences.begin(); iter != __evidences.end(); ++iter ) {
    //   pool.insert( const_cast< Potential<T_DATA>* >( *iter ) );
    // }
    for( size_t i = 0; i < __eliminationOrder.size(); ++i ) {
      if( __eliminationOrder[i] != id ) {
        __eliminateNode( __eliminationOrder[i], pool, __trash );
      }
    }

    try {
      pool.insert( const_cast<Potential<T_DATA>*>( __evidences[id] ) );
    } catch( NotFound& ) {
      // No evidence on query
    }

    marginal.add( this->bn().variable( id ) );
    marginal.fill( ( T_DATA ) 1 );

    for( SetIterator< Potential<T_DATA>* > iter = pool.begin(); iter != pool.end(); ++iter ) {
      if( ( **iter ).nbrDim() > 0 ) {
        marginal.multiplicateBy( **iter );
      }
    }

    marginal.normalize();

    // Cleaning up the mess
    for( SetIterator< Potential<T_DATA>* > iter = __trash.begin(); iter != __trash.end(); ++iter ) {
      delete *iter;
    }

    __trash.clear();
  }

  template<typename T_DATA>
  void
  VariableElimination<T_DATA>::__computeEliminationOrder() {
    if( __eliminationOrder.empty() ) {
      typename Property<unsigned int>::onNodes modalities;

      for( DAG::NodeIterator iter = this->bn().beginNodes(); iter != this->bn().endNodes(); ++iter ) {
        modalities.insert( *iter, this->bn().variable( *iter ).domainSize() );
      }

      DefaultTriangulation triang( &( this->bn().moralGraph() ), &modalities );
      __eliminationOrder = triang.eliminationOrder();
    }
  }

  template<typename T_DATA>
  void
  VariableElimination<T_DATA>::__createInitialPool() {
    __pool.clear();

    for( DAG::NodeIterator iter = this->bn().beginNodes(); iter != this->bn().endNodes(); ++iter ) {
      __pool.insert( const_cast< Potential<T_DATA>* >( &( this->bn().cpt( *iter ) ) ) );

      if( __evidences.exists( *iter ) )
        __pool.insert( const_cast< Potential<T_DATA>* >( __evidences[*iter] ) );
    }
  }
  // the function used to combine two tables
  template <typename T_DATA>
  Potential<T_DATA>* multPotential ( const Potential<T_DATA>& t1,
                                     const Potential<T_DATA>& t2 ) {
    return new Potential<T_DATA> (t1 * t2);
                                    }

  template<typename T_DATA>
  void
  VariableElimination<T_DATA>::__eliminateNode( NodeId id,
      Set< Potential<T_DATA>* >& pool,
      Set< Potential<T_DATA>* >& trash ) {
    // THIS IS A (TEMPLATIZED) COPY OF prm::eliminateNode
    {
      const gum::DiscreteVariable* var=&(this->bn().variable(id));

      Potential<T_DATA>* pot = 0;
      Potential<T_DATA>* tmp = 0;
      Set<const DiscreteVariable*> var_set;
      var_set.insert( var );
      Set<const Potential<T_DATA>*> pots;

      for( SetIterator<Potential<T_DATA>*> iter = pool.begin(); iter != pool.end(); ++iter )
        if( ( *iter )->contains( *var ) )
          pots.insert( *iter );

      if( pots.size() == 0 ) {
        return;
      } else if( pots.size() == 1 ) {
        tmp = const_cast<Potential<T_DATA>*>( *( pots.begin() ) );
        pot = new Potential<T_DATA>( projectSum( *tmp, var_set ) );
      } else {
        MultiDimCombinationDefault<T_DATA,Potential> Comb( multPotential );
        tmp = Comb.combine( pots );
        pot = new Potential<T_DATA>( projectSum( *tmp, var_set ) );
        delete tmp;
      }

      for( typename Set<const Potential<T_DATA>*>::iterator iter = pots.begin(); iter != pots.end(); ++iter ) {
        pool.erase( const_cast<Potential<T_DATA>*>( *iter ) );

        if( trash.exists( const_cast<Potential<T_DATA>*>( *iter ) ) ) {
          trash.erase( const_cast<Potential<T_DATA>*>( *iter ) );
          delete const_cast<Potential<T_DATA>*>( *iter );
        }
      }

      pool.insert( pot );
      trash.insert( pot );
    }
  }

} /* namespace gum */
// ============================================================================
