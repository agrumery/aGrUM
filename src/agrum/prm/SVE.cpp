/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Implementation of SVE.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/SVE.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/SVE.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {


SVE::~SVE() {
  GUM_DESTRUCTOR(SVE);
  typedef HashTable<const Class*, std::vector<NodeId>*>::iterator ElimIter;
  for (ElimIter iter = __elim_orders.begin(); iter != __elim_orders.end(); ++iter) {
    delete *iter;
  }
  typedef HashTable<const Class*, SVE::ArraySet*>::iterator LiftedIter;
  for (LiftedIter iter = __lifted_pools.begin(); iter != __lifted_pools.end(); ++iter) {
    for (SVE::ArraySetIterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
      delete *jter;
    }
    delete *iter;
  }
  if (__class_elim_order != 0) {
    delete __class_elim_order;
  }
  for (SVE::BucketSetIterator iter = __lifted_trash.begin(); iter != __lifted_trash.end(); ++iter) {
    delete *iter;
  }
}

void
SVE::__eliminateNodes(const Instance* query, NodeId node, BucketSet& pool, BucketSet& trash) {
  // {
  //   std::stringstream sBuff;
  //   sBuff << "__eliminateNodes: " << query->name() << ", " << query->get(node).safeName();
  //   sBuff << ", " << pool.size() << ", " << trash.size();
  // }
  Set<const Instance*> ignore;
  // Downward elimination
  List<const Instance*> elim_list;
  ignore.insert(query);
  for (Instance::InvRefConstIterator iter = query->beginInvRef(); iter != query->endInvRef(); ++iter) {
    typedef std::vector< std::pair<Instance*, std::string> >::const_iterator pair_iter;
    for (pair_iter child = (**iter).begin(); child != (**iter).end(); ++child) {
      if (not ignore.exists(child->first)) {
        __eliminateNodesDownward(query, child->first, pool, trash, elim_list, ignore);
      }
    }
  }
  // Eliminating all nodes in query instance, except query
  InstanceBayesNet bn(*query);
  DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
  std::vector<NodeId> elim_order;
  VariableElimination<prm_float> inf(bn);
  if (hasEvidence(query)) {
    __insertEvidence(query, pool);
  }
  for (Instance::const_iterator attr = query->begin(); attr != query->end(); ++attr) {
    pool.insert(&(const_cast<Potential<prm_float>&>((**attr).cpf())));
  }
  for (size_t idx = 0; idx < t.eliminationOrder().size(); ++idx) {
    if (t.eliminationOrder()[idx] != node) {
      elim_order.push_back(t.eliminationOrder()[idx]);
    }
  }
  inf.eliminateNodes(elim_order, pool, trash);
  // Eliminating instance in elim_list
  List<const Instance*> tmp_list;
  while (not elim_list.empty()) {
    if (__checkElimOrder(query, elim_list.front())) {
      if (not ignore.exists(elim_list.front())) {
        __eliminateNodesDownward(query, elim_list.front(), pool, trash, elim_list, ignore);
      }
    } else {
      tmp_list.insert(elim_list.front());
    }
    elim_list.popFront();
  }
  // Upward elimination
  for (Set<SlotChain*>::iterator sc = query->type().slotChains().begin(); sc != query->type().slotChains().end(); ++sc) {
    for (Set<Instance*>::iterator parent = query->getInstances((**sc).id()).begin(); parent != query->getInstances((**sc).id()).end(); ++parent) {
      if (not ignore.exists(*parent)) {
        __eliminateNodesUpward(*parent, pool, trash, tmp_list, ignore);
      }
    }
  }
}

void
SVE::__eliminateNodesDownward(const Instance* from, const Instance* i, BucketSet& pool, BucketSet& trash, List<const Instance*>& elim_list, Set<const Instance*>& ignore)
{
  // {
  //   std::stringstream sBuff;
  //   sBuff << "__eliminateNodesDownward: " << from->name() << ", " << i->name();
  //   sBuff << ", " << pool.size() << ", " << trash.size();
  //   GUM_TRACE(sBuff.str());
  // }
  ignore.insert(i);
  // Calling elimination over child instance
  List<const Instance*> my_list;
  for (Instance::InvRefConstIterator iter = i->beginInvRef(); iter != i->endInvRef(); ++iter) {
    typedef std::vector< std::pair<Instance*, std::string> >::const_iterator pair_iter;
    for (pair_iter child = (**iter).begin(); child != (**iter).end(); ++child) {
      if (not ignore.exists(child->first)) {
        __eliminateNodesDownward(i, child->first, pool, trash, my_list, ignore);
      }
    }
  }
  // Eliminating all nodes in current instance
  InstanceBayesNet bn(*i);
  VariableElimination<prm_float> inf(bn);
  if (hasEvidence(i)) {
    __eliminateNodesWithEvidence(i, pool, trash);
  } else {
    __insertLiftedNodes(i, pool, trash);
    for (Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg) {
      pool.insert(__getAggPotential(i, *agg));
    }
    try {
      inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
    } catch (NotFound&) {
      // Raised if there is no inner nodes to eliminate
    }
  }
  // Calling elimination over child's parents
  for (List<const Instance*>::iterator iter = my_list.begin(); iter != my_list.end(); ++iter) {
    if (__checkElimOrder(i, *iter) and (*iter != from)) {
      if (not ignore.exists(*iter)) {
        __eliminateNodesDownward(i, *iter, pool, trash, elim_list, ignore);
      }
    } else if (*iter != from) {
      elim_list.insert(*iter);
    }
  }
  // Adding parents instance to elim_list
  for (Set<SlotChain*>::iterator sc = i->type().slotChains().begin(); sc != i->type().slotChains().end(); ++sc) {
    for (Set<Instance*>::iterator iter = i->getInstances((**sc).id()).begin(); iter != i->getInstances((**sc).id()); ++iter) {
      if (*iter != from) {
        elim_list.insert(*iter);
      }
    }
  }
}

void
SVE::__eliminateNodesUpward(const Instance* i, BucketSet& pool, BucketSet& trash, List<const Instance*>& elim_list, Set<const Instance*>& ignore)
{
  // {
  //   std::stringstream sBuff;
  //   sBuff << "__eliminateNodesUpward: " << i->name();
  //   sBuff << ", " << pool.size() << ", " << trash.size();
  //   GUM_TRACE(sBuff.str());
  // }
  // Downward elimination
  ignore.insert(i);
  for (Instance::InvRefConstIterator iter = i->beginInvRef(); iter != i->endInvRef(); ++iter) {
    typedef std::vector< std::pair<Instance*, std::string> >::const_iterator pair_iter;
    for (pair_iter child = (**iter).begin(); child != (**iter).end(); ++child) {
      if (not ignore.exists(child->first)) {
        __eliminateNodesDownward(i, child->first, pool, trash, elim_list, ignore);
      }
    }
  }
  // Eliminating all nodes in i instance
  InstanceBayesNet bn(*i);
  VariableElimination<prm_float> inf(bn);
  if (hasEvidence(i)) {
    __eliminateNodesWithEvidence(i, pool, trash);
  } else {
    __insertLiftedNodes(i, pool, trash);
    for (Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg) {
      pool.insert(__getAggPotential(i, *agg));
    }
    try {
      inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
    } catch (NotFound&) {
      // Raised if there is no inner nodes to eliminate
    }
  }
  // Eliminating instance in elim_list
  List<const Instance*> tmp_list;
  while (not elim_list.empty()) {
    if (__checkElimOrder(i, elim_list.front())) {
      if (not ignore.exists(elim_list.front())) {
        __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list, ignore);
      }
    } else {
      tmp_list.insert(elim_list.front());
    }
    elim_list.popFront();
  }
  // Upward elimination
  for (Set<SlotChain*>::iterator sc = i->type().slotChains().begin(); sc != i->type().slotChains().end(); ++sc) {
    for (Set<Instance*>::iterator parent = i->getInstances((**sc).id()).begin(); parent != i->getInstances((**sc).id()).end(); ++parent) {
      if (not ignore.exists(*parent)) {
        __eliminateNodesUpward(*parent, pool, trash, tmp_list, ignore);
      }
    }
  }
}

void
SVE::__eliminateNodesWithEvidence(const Instance* i, BucketSet& pool, BucketSet& trash) {
  // First we check if evidences are on inner nodes
  bool inner = false;
  for (EMapIterator e = evidence(i).begin(); e != evidence(i).end(); ++e) {
    inner = inner or i->type().isInputNode(i->get(e.key())) or i->type().isInnerNode(i->get(e.key()));
    if (inner) { break; }
  }
  if (inner) {
    BucketSet tmp_pool;
    __insertEvidence(i, tmp_pool);
    // We need a local to not eliminat queried inner nodes of the same class
    for (Instance::const_iterator attr = i->begin(); attr != i->end(); ++attr) {
      tmp_pool.insert(&((**attr).cpf()));
    }
    InstanceBayesNet bn(*i);
    DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
    const std::vector<NodeId>& full_elim_order = t.eliminationOrder();
    VariableElimination<prm_float> inf(bn);
    // Removing Output nodes of elimination order
    std::vector<NodeId> inner_elim_order;
    std::vector<NodeId> output_elim_order;
    for (size_t idx = 0; idx < full_elim_order.size(); ++idx) {
      if (not i->type().isOutputNode(i->get(full_elim_order[idx]))) {
        inner_elim_order.push_back(full_elim_order[idx]);
      } else {
        output_elim_order.push_back(full_elim_order[idx]);
      }
    }
    inf.eliminateNodes(inner_elim_order, tmp_pool, trash);
    // Now we add the new potentials in pool and eliminate output nodes
    for (BucketSetIterator iter = tmp_pool.begin(); iter != tmp_pool.end(); ++iter) {
      pool.insert(*iter);
    }
    if (not output_elim_order.empty()) {
      inf.eliminateNodes(output_elim_order, pool, trash);
    }
  } else {
    InstanceBayesNet bn(*i);
    VariableElimination<prm_float> inf(bn);
    __insertEvidence(i, pool);
    __insertLiftedNodes(i, pool, trash);
    for (Set<Aggregate*>::iterator agg = i->type().aggregates().begin();
        agg != i->type().aggregates().end(); ++agg) {
      pool.insert(__getAggPotential(i, *agg));
    }
    try {
      inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
    } catch (NotFound&) {
      GUM_ERROR(FatalError, "there should be at least one node here.");
    }
  }
}

void
SVE::__insertLiftedNodes(const Instance* i, BucketSet& pool, BucketSet& trash) {
  SVE::ArraySet* lifted_pool = 0;
  try {
    lifted_pool = __lifted_pools[&(i->type())];
  } catch (NotFound&) {
    __initLiftedNodes(i->type());
    lifted_pool = __lifted_pools[&(i->type())];
  }
  for (SVE::ArraySetIterator iter = lifted_pool->begin(); iter != lifted_pool->end(); ++iter) {
    Potential<prm_float>* pot = new Potential<prm_float>( new MultiDimBijArray<prm_float>(i->bijection(), **iter));
    pool.insert(pot);
    trash.insert(pot);
  }
}

void
SVE::__initLiftedNodes(const Class& c)
{
  BucketSet lifted_pool;
  __lifted_pools.insert(&c, new SVE::ArraySet());
  for (Set<Attribute*>::iterator attr = c.attributes().begin(); attr != c.attributes().end(); ++attr) {
    lifted_pool.insert(&((**attr).cpf()));
  }
  ClassBayesNet bn(c);
  DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
  const std::vector<NodeId>& full_elim_order = t.eliminationOrder();
  VariableElimination<prm_float> inf(bn);
  // Removing Output nodes of elimination order
  std::vector<NodeId> inner_elim_order;
  std::vector<NodeId>* output_elim_order = new std::vector<NodeId>();
  for (size_t idx = 0; idx < full_elim_order.size(); ++idx) {
    if (c.isOutputNode(c.get(full_elim_order[idx])) or
        ClassElement::isAggregate(c.get(full_elim_order[idx]))) {
      output_elim_order->push_back(full_elim_order[idx]);
    } else {
      inner_elim_order.push_back(full_elim_order[idx]);
    }
  }
  // If there is only output nodes and Aggregate we can't lift anything
  if (not inner_elim_order.empty()) {
    inf.eliminateNodes(inner_elim_order, lifted_pool, __lifted_trash);
  }
  // Copying buckets in MultiDimArrays
  for(SVE::BucketSetIterator iter = lifted_pool.begin(); iter != lifted_pool.end(); ++iter) {
    const MultiDimBucket<prm_float>* b = 0;
    const MultiDimImplementation<prm_float>* impl = const_cast<const Potential<prm_float>&>((**iter)).getContent();
    b = dynamic_cast<const MultiDimBucket<prm_float>* >(impl);
    if (b != 0) {
      b->compute(true);
      try {
        __lifted_pools[&c]->insert(new MultiDimArray<prm_float>(b->bucket()));
      } catch (OperationNotAllowed& e) {
        // Empty bucket
      }
    } else {
      const MultiDimArray<prm_float>* a = dynamic_cast<const MultiDimArray<prm_float>*>(impl);
      if (a != 0) {
        __lifted_pools[&c]->insert(new MultiDimArray<prm_float>(*a));
      } else {
        GUM_ERROR(FatalError, "unknown content");
      }
    }
  }
  // If there is not only inner and input Attributes
  if (not output_elim_order->empty()) {
    __elim_orders.insert(&c, output_elim_order);
  } else {
    delete output_elim_order;
  }
}

void
SVE::__initElimOrder() {
  CDG cdg(*_prm);
  __class_elim_order = new Sequence<const Class*>();
  std::list<NodeId> l;
  for (DAG::NodeIterator node = cdg.dag().beginNodes();
      node != cdg.dag().endNodes(); ++node) {
    if (cdg.dag().parents(*node).empty()) {
      l.push_back(*node);
    }
  }
  Set<NodeId> visited_node;
  while (not l.empty()) {
    visited_node.insert(l.front());
    if (not __class_elim_order->exists(cdg.get(l.front()).first)) {
      __class_elim_order->insert(cdg.get(l.front()).first);
    }
    for (DAG::ArcIterator child = cdg.dag().children(l.front()).begin();
        child != cdg.dag().children(l.front()).end(); ++child) {
      if (not visited_node.contains(child->head())) {
        l.push_back(child->head());
      }
    }
    l.pop_front();
  }
}

void
SVE::_marginal(const Chain& chain, Potential<prm_float>& m)
{
  const Instance* i = chain.first;
  const Attribute* elt = chain.second;
  SVE::BucketSet pool, trash;
  __eliminateNodes(i, elt->id(), pool, trash);
  m.fill((prm_float) 1);
  for (SVE::BucketSetIterator iter = pool.begin(); iter != pool.end(); ++iter) {
    if ((**iter).contains(elt->type().variable())) {
      m.multiplicateBy(**iter);
    }
  }
  m.normalize();
  for (SVE::BucketSetIterator iter = trash.begin(); iter != trash.end(); ++iter) {
    delete *iter;
  }
}

void
SVE::_joint(const std::vector<Chain>& queries, Potential<prm_float>& j) {
  GUM_ERROR(FatalError, "Not implemented.");
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
