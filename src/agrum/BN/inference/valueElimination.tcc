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
namespace gum {

// Default constructor.
template<typename T_DATA> INLINE
ValueElimination<T_DATA>::ValueElimination(const BayesNet<T_DATA>& bn):
  BayesNetInference<T_DATA>(bn)
{
  GUM_CONSTRUCTOR( ValueElimination );
}

// Destructor.
template<typename T_DATA> INLINE
ValueElimination<T_DATA>::~ValueElimination()
{
  GUM_DESTRUCTOR( ValueElimination );
}

// Returns a constant reference over the sequence used as order elimination.
template<typename T_DATA> INLINE
const std::vector<NodeId>&
ValueElimination<T_DATA>::eliminationOrder() const
{
  if (__eliminationOrder.size() != 0) return __eliminationOrder;
  else GUM_ERROR(OperationNotAllowed, "Elimination order uninitialized.");
}

// @brief Force the elimination order to follow elim.
// This method checks that all nodes present in the BN are in elim.
// @throw OperationNotAllowed Raised if elim does not contain all the bayes net's nodes.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::setEliminiationOrder(const std::vector<NodeId>& elim)
{
  __eliminationOrder = elim;
}

// @brief Makes the inference
// this method only computes the elimination order if needed, and proceed to
// some basic initialization.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::makeInference()
{
  __computeEliminationOrder();
  __createInitialPool();
}

// @brief Insert new evidence in the graph.
// If an evidence already exists over one of the variable in pot_list, then
// it is replaced by the new evidence in pot_list.
// @throw OperationNotAllowed Raised if an evidence is over more than one variable.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::insertEvidence( const List<const Potential<T_DATA>*>& pot_list )
{
  NodeId varId;
  for (ListIterator< const Potential<T_DATA>* > iter = pot_list.begin(); iter != pot_list.end(); ++iter) {
    if (( *iter )->nbrDim() != 1 ) {
      GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
    }
    varId = BayesNetInference<T_DATA>::__bayesNet.nodeId((*iter)->variable(0));
    try {
      __evidences[varId] = *iter;
    } catch (NotFound&) {
      __evidences.insert(varId, *iter);
    }
  }
}

// Remove a given evidence from the graph.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::eraseEvidence( const Potential<T_DATA>* e )
{
  if (e->nbrDim() != 1 ) {
    GUM_ERROR( OperationNotAllowed, "Evidence can only be giben w.r.t. one random variable" );
  }
  __evidences.erase(BayesNetInference<T_DATA>::__bayesNet.nodeId(e->variable(0)));
}

// Remove all evidence from the graph.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::eraseAllEvidence()
{
  __evidences.clear();
}

// Returns the probability of the variable.
// @param id The variable's id.
// @param marginal the potential to fill
// @throw ElementNotFound Raised if no variable matches id.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::_fillMarginal(NodeId id, Potential<T_DATA>& marginal)
{
  __computeEliminationOrder();
  __createInitialPool();
  Set< Potential<T_DATA>* > pool(__pool);
  for (typename Property< const Potential<T_DATA>* >::onNodes::iterator iter = __evidences.begin(); iter != __evidences.end(); ++iter) {
    pool.insert(const_cast< Potential<T_DATA>* >(*iter));
  }
  for (size_t i = 0; i < __eliminationOrder.size(); ++i) {
    if (__eliminationOrder[i] != id) {
      MultiDimBucket<T_DATA>* bucket = new MultiDimBucket<T_DATA>();
      __eliminateNode(__eliminationOrder[i], *bucket, pool);
    }
  }
  marginal.add(BayesNetInference<T_DATA>::__bayesNet.variable(id));
  marginal.fill((T_DATA) 1);
  for (SetIterator< Potential<T_DATA>* > iter = pool.begin(); iter != pool.end(); ++iter) {
    marginal.multiplicateBy(**iter);
  }
  marginal.normalize();
  // Cleaning up the mess
  for (SetIterator< Potential<T_DATA>* > iter = __trash.begin(); iter != __trash.end(); ++iter) {
    delete *iter;
  }
  __trash.clear();
}

// Compute the elimination order if __eliminationOrder is empty.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::__computeEliminationOrder()
{
  if (__eliminationOrder.empty()) {
    typename Property<unsigned int>::onNodes modalities;
    for (NodeSetIterator iter = BayesNetInference<T_DATA>::__bayesNet.beginNodes(); iter != BayesNetInference<T_DATA>::__bayesNet.endNodes(); ++iter) {
      modalities.insert(*iter, BayesNetInference<T_DATA>::__bayesNet.variable(*iter).domainSize());
    }
    DefaultTriangulation triang((const DiGraph&) BayesNetInference<T_DATA>::__bayesNet.dag(), modalities);
    __eliminationOrder = triang.eliminationOrder();
  }
}

// Fills the pool with all the potentials in __bayesNet.
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::__createInitialPool()
{
  if (__pool.empty()) {
    for (NodeSetIterator iter = BayesNetInference<T_DATA>::__bayesNet.beginNodes(); iter != BayesNetInference<T_DATA>::__bayesNet.endNodes(); ++iter) {
      __pool.insert(const_cast< Potential<T_DATA>* >(&BayesNetInference<T_DATA>::__bayesNet.cpt(*iter)));
    }
  }
}

// Fills the bucket with all the potentials in pool containing id and
// insert it in pool as a Potential after removing all the potentials
// already in it. If you don't understand, read the code...
template<typename T_DATA> INLINE
void
ValueElimination<T_DATA>::__eliminateNode(NodeId id,
                                          MultiDimBucket<T_DATA>& bucket,
                                          Set< Potential<T_DATA>* >& pool)
{
  Set< Potential<T_DATA>* > toRemove;
  for (SetIterator<Potential<T_DATA>*> iter = pool.begin(); iter != pool.end(); ++iter) {
    if ((*iter)->contains(BayesNetInference<T_DATA>::__bayesNet.variable(id))) {
      bucket.add(**iter);
      toRemove.insert(*iter);
    }
  }
  for (SetIterator< Potential<T_DATA>* > iter = toRemove.begin(); iter != toRemove.end(); ++iter) {
    pool.erase(*iter);
  }
  Potential<T_DATA>* bucket_pot = new Potential<T_DATA>(&bucket);
  __trash.insert(bucket_pot);
  pool.insert(bucket_pot);
  for (Set<const DiscreteVariable*>::iterator jter = bucket.allVariables().begin(); jter != bucket.allVariables().end(); ++jter) {
    if (BayesNetInference<T_DATA>::__bayesNet.nodeId(**jter) != id) {
      bucket_pot->add(**jter);
    }
  }
}

} /* namespace gum */
// ============================================================================
