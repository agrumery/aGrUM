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
VariableElimination<T_DATA>::VariableElimination(const VariableElimination<T_DATA>& source) {
  GUM_CONS_CPY( VariableElimination );
  GUM_ERROR(FatalError, "illegal call to VariableElimination copy constructor.");
}

template<typename T_DATA> INLINE
VariableElimination<T_DATA>&
VariableElimination<T_DATA>::operator=(const VariableElimination& source) {
  GUM_ERROR(FatalError, "illegal call to VariableElimination copy operator.");
}

template<typename T_DATA> INLINE
const std::vector<NodeId>&
VariableElimination<T_DATA>::eliminationOrder() const {
  if ( __eliminationOrder.size() != 0 ) {
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
  for ( ListConstIterator< const Potential<T_DATA>* > iter = pot_list.begin(); iter != pot_list.end(); ++iter ) {
    if (( *iter )->nbrDim() != 1 ) {
      GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
    }
    NodeId varId = this->bn().nodeId(( *iter )->variable( 0 ) );
    try {
      __evidences[varId] = *iter;
    } catch ( NotFound& ) {
      __evidences.insert( varId, *iter );
    }
  }
  this->_invalidateMarginals();
}

template<typename T_DATA> INLINE
void
VariableElimination<T_DATA>::eraseEvidence( const Potential<T_DATA>* e ) {
  if ( e->nbrDim() != 1 ) {
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
VariableElimination<T_DATA>::eliminateNodes(const std::vector<NodeId>& elim_order,
                                         Set< Potential<T_DATA>* >& pool,
                                         Set< Potential<T_DATA>* >& trash) {
  for ( size_t i = 0; i < elim_order.size(); ++i ) {
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
  for ( size_t i = 0; i < __eliminationOrder.size(); ++i ) {
    if ( __eliminationOrder[i] != id ) {
      __eliminateNode( __eliminationOrder[i], pool, __trash );
    }
  }
  try {
    pool.insert(const_cast<Potential<T_DATA>*>(__evidences[id]));
  } catch (NotFound&) {
    // No evidence on query
  }
  marginal.add( this->bn().variable( id ) );
  marginal.fill(( T_DATA ) 1 );
  for ( SetIterator< Potential<T_DATA>* > iter = pool.begin(); iter != pool.end(); ++iter ) {
    if ((**iter).nbrDim() > 0) {
      marginal.multiplicateBy( **iter );
    }
  }
  marginal.normalize();
  // Cleaning up the mess
  for ( SetIterator< Potential<T_DATA>* > iter = __trash.begin(); iter != __trash.end(); ++iter ) {
    delete *iter;
  }
  __trash.clear();
}

template<typename T_DATA>
void
VariableElimination<T_DATA>::__computeEliminationOrder() {
  if ( __eliminationOrder.empty() ) {
    typename Property<unsigned int>::onNodes modalities;
    for ( DAG::NodeIterator iter = this->bn().beginNodes(); iter != this->bn().endNodes(); ++iter ) {
      modalities.insert( *iter, this->bn().variable( *iter ).domainSize() );
    }
    DefaultTriangulation triang(&(this->bn().moralGraph()), &modalities );
    __eliminationOrder = triang.eliminationOrder();
  }
}

template<typename T_DATA>
void
VariableElimination<T_DATA>::__createInitialPool() {
  __pool.clear();
  for ( DAG::NodeIterator iter = this->bn().beginNodes(); iter != this->bn().endNodes(); ++iter ) {
    __pool.insert( const_cast< Potential<T_DATA>* >( &(this->bn().cpt(*iter)) ) );
  }
}

template<typename T_DATA>
void
VariableElimination<T_DATA>::__eliminateNode( NodeId id,
                                           Set< Potential<T_DATA>* >& pool,
                                           Set< Potential<T_DATA>* >& trash ) {
  MultiDimBucket<T_DATA>* bucket = new MultiDimBucket<T_DATA>();
  Set< Potential<T_DATA>* > toRemove;
  for ( SetIterator<Potential<T_DATA>*> iter = pool.begin(); iter != pool.end(); ++iter ) {
    if (( *iter )->contains( this->bn().variable( id ) ) ) {
      bucket->add( **iter );
      toRemove.insert( *iter );
    }
  }
  try {
    bucket->add(__evidences[id]);
  } catch (NotFound&) {
    // No evidence on id
  }
  if (not toRemove.empty()) {
    for ( SetIterator< Potential<T_DATA>* > iter = toRemove.begin(); iter != toRemove.end(); ++iter ) {
      pool.erase( *iter );
    }
    for (Set<const DiscreteVariable*>::iterator jter = bucket->allVariables().begin();
         jter != bucket->allVariables().end(); ++jter ) {
      try {
        if ( this->bn().nodeId( **jter ) != id ) {
          bucket->add( **jter );
        }
      } catch (NotFound&) {
        // This can happen if bn is a HollowBayesNet...
        bucket->add(**jter);
      }
    }
    Potential<T_DATA>* bucket_pot = new Potential<T_DATA>( bucket );
    trash.insert( bucket_pot );
    pool.insert( bucket_pot );
  } else {
    delete bucket;
  }
}

} /* namespace gum */
// ============================================================================
