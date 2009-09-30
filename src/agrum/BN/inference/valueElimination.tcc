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
#include <agrum/BN/inference/valueElimination.h>
// ============================================================================
namespace gum {

template<typename T_DATA> INLINE
ValueElimination<T_DATA>::ValueElimination( const AbstractBayesNet<T_DATA>& bn ):
    BayesNetInference<T_DATA>( bn ) {
  GUM_CONSTRUCTOR( ValueElimination );
}

template<typename T_DATA> INLINE
ValueElimination<T_DATA>::~ValueElimination() {
  GUM_DESTRUCTOR( ValueElimination );
}

template<typename T_DATA> INLINE
const std::vector<NodeId>&
ValueElimination<T_DATA>::eliminationOrder() const {
  if ( __eliminationOrder.size() != 0 ) {
    return __eliminationOrder;
  } else {
    GUM_ERROR( OperationNotAllowed, "Elimination order uninitialized." );
  }
}

template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::setEliminiationOrder( const std::vector<NodeId>& elim ) {
  __eliminationOrder = elim;
}

template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::makeInference() {
  __computeEliminationOrder();
  __createInitialPool();
}

template<typename T_DATA>
void
ValueElimination<T_DATA>::insertEvidence( const List<const Potential<T_DATA>*>& pot_list ) {
  for ( ListConstIterator< const Potential<T_DATA>* > iter = pot_list.begin(); iter != pot_list.end(); ++iter ) {
    if (( *iter )->nbrDim() != 1 ) {
      GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
    }
    NodeId varId = BayesNetInference<T_DATA>::__bayesNet.nodeId(( *iter )->variable( 0 ) );
    try {
      __evidences[varId] = *iter;
    } catch ( NotFound& ) {
      __evidences.insert( varId, *iter );
    }
  }
  BayesNetInference<T_DATA>::_invalidateMarginals();
}

template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::eraseEvidence( const Potential<T_DATA>* e ) {
  if ( e->nbrDim() != 1 ) {
    GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
  }
  __evidences.erase( BayesNetInference<T_DATA>::__bayesNet.nodeId( e->variable( 0 ) ) );
  BayesNetInference<T_DATA>::_invalidateMarginals();
}

template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::eraseAllEvidence() {
  __evidences.clear();
  BayesNetInference<T_DATA>::_invalidateMarginals();
}

template<typename T_DATA>
void
ValueElimination<T_DATA>::_fillMarginal( NodeId id, Potential<T_DATA>& marginal ) {
  __computeEliminationOrder();
  __createInitialPool();
  Set< Potential<T_DATA>* > pool( __pool );
  for ( typename Property< const Potential<T_DATA>* >::onNodes::iterator iter = __evidences.begin(); iter != __evidences.end(); ++iter ) {
    pool.insert( const_cast< Potential<T_DATA>* >( *iter ) );
  }
  for ( size_t i = 0; i < __eliminationOrder.size(); ++i ) {
    if ( __eliminationOrder[i] != id ) {
      MultiDimBucket<T_DATA>* bucket = new MultiDimBucket<T_DATA>();
      __eliminateNode( __eliminationOrder[i], *bucket, pool );
    }
  }
  marginal.add( BayesNetInference<T_DATA>::__bayesNet.variable( id ) );
  marginal.fill(( T_DATA ) 1 );
  for ( SetIterator< Potential<T_DATA>* > iter = pool.begin(); iter != pool.end(); ++iter ) {
    marginal.multiplicateBy( **iter );
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
ValueElimination<T_DATA>::__computeEliminationOrder() {
  if ( __eliminationOrder.empty() ) {
    typename Property<unsigned int>::onNodes modalities;
    for ( DAG::NodeIterator iter = BayesNetInference<T_DATA>::__bayesNet.beginNodes(); iter != BayesNetInference<T_DATA>::__bayesNet.endNodes(); ++iter ) {
      modalities.insert( *iter, BayesNetInference<T_DATA>::__bayesNet.variable( *iter ).domainSize() );
    }
    DefaultTriangulation triang(( const DiGraph& ) BayesNetInference<T_DATA>::__bayesNet.dag(), modalities );
    __eliminationOrder = triang.eliminationOrder();
  }
}

template<typename T_DATA>
void
ValueElimination<T_DATA>::__createInitialPool() {
  __pool.clear();
  for ( DAG::NodeIterator iter = BayesNetInference<T_DATA>::__bayesNet.beginNodes(); iter != BayesNetInference<T_DATA>::__bayesNet.endNodes(); ++iter ) {
    __pool.insert( const_cast< Potential<T_DATA>* >( &BayesNetInference<T_DATA>::__bayesNet.cpt( *iter ) ) );
  }
}

template<typename T_DATA>
void
ValueElimination<T_DATA>::__eliminateNode( NodeId id, MultiDimBucket<T_DATA>& bucket, Set< Potential<T_DATA>* >& pool ) {
  Set< Potential<T_DATA>* > toRemove;
  int count = 0;
  for ( SetIterator<Potential<T_DATA>*> iter = pool.begin(); iter != pool.end(); ++iter ) {
    if (( *iter )->contains( BayesNetInference<T_DATA>::__bayesNet.variable( id ) ) ) {
      bucket.add( **iter );
      toRemove.insert( *iter );
      count++;
    }
  }
  try {
    bucket.add(__evidences[id]);
    count++;
  } catch (NotFound&) {
    // No evidence on id
  }
  for ( SetIterator< Potential<T_DATA>* > iter = toRemove.begin(); iter != toRemove.end(); ++iter ) {
    pool.erase( *iter );
  }
  Potential<T_DATA>* bucket_pot = new Potential<T_DATA>( &bucket );
  __trash.insert( bucket_pot );
  pool.insert( bucket_pot );
  for ( Set<const DiscreteVariable*>::iterator jter = bucket.allVariables().begin(); jter != bucket.allVariables().end(); ++jter ) {
    if ( BayesNetInference<T_DATA>::__bayesNet.nodeId( **jter ) != id ) {
      bucket_pot->add( **jter );
    }
  }
}

} /* namespace gum */
// ============================================================================
