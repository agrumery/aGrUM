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
 * @brief Implementation of SVED.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/SVED.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/SVED.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {


SVED::~SVED() {
  GUM_DESTRUCTOR(SVED);
  typedef HashTable<const Class*, std::vector<NodeId>*>::iterator ElimIter;
  for (ElimIter i = __elim_orders.begin(); i != __elim_orders.end(); ++i) {
    delete *i;
  }
  if (__class_elim_order != 0) {
    delete __class_elim_order;
  }
}

void
SVED::__eliminateNodes(const Instance* query, NodeId node, BucketSet& pool, BucketSet& trash)
{
  Set<const Instance*> ignore;
  ignore.insert(query);
  // Extracting required attributes and slotchains
  Set<NodeId>& attr_set = __getAttrSet(query);
  Set<NodeId>& sc_set = __getSCSet(query);
  // Downward elimination
  List<const Instance*> elim_list;
  for (Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr) {
    try {
      typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
      for (Iter iter = query->getRefAttr(*attr).begin(); iter != query->getRefAttr(*attr).end(); ++iter) {
        if ( (not ignore.exists(iter->first)) and (__bb.exists(iter->first)) ) {
          __eliminateNodesDownward(query, iter->first, pool, trash, elim_list, ignore);
        }
      }
    } catch (NotFound&) {
      // Ok
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
  for (Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr) {
    pool.insert(&(const_cast<Potential<prm_float>&>(query->get(*attr).cpf())));
  }
  for (size_t idx = 0; idx < t.eliminationOrder().size(); ++idx) {
    if (t.eliminationOrder()[idx] != node) {
      elim_order.push_back(t.eliminationOrder()[idx]);
    }
  }
  inf.eliminateNodes(elim_order, pool, trash);
  // Eliminating instance in elim_list
  List<const Instance*> tmp_list;
  __reduceElimList(query, elim_list, tmp_list, ignore, pool, trash);
  while (not elim_list.empty()) {
    if (__checkElimOrder(query, elim_list.front())) {
      if ( (not ignore.exists(elim_list.front())) and (__bb.exists(elim_list.front())) ) {
        __eliminateNodesDownward(query, elim_list.front(), pool, trash, elim_list, ignore);
      }
    } else if (__bb.exists(elim_list.front())) {
      tmp_list.insert(elim_list.front());
    }
    elim_list.popFront();
  }
  // Upward elimination
  for (Set<NodeId>::iterator sc = sc_set.begin(); sc != sc_set.end(); ++sc) {
    for (Set<Instance*>::iterator parent = query->getInstances(*sc).begin(); parent != query->getInstances(*sc).end(); ++parent) {
      if ( (not ignore.exists(*parent)) and (__bb.exists(*parent)) ) {
        __eliminateNodesUpward(*parent, pool, trash, tmp_list, ignore);
      }
    }
  }
}

void
SVED::__eliminateNodesDownward(const Instance* from, const Instance* i,
                               BucketSet& pool, BucketSet& trash,
                               List<const Instance*>& elim_list,
                               Set<const Instance*>& ignore)
{
  ignore.insert(i);
  // Extracting required attributes and slotchains
  Set<NodeId>& attr_set = __getAttrSet(i);
  Set<NodeId>& sc_set   = __getSCSet(i);
  // Calling elimination over child instance
  List<const Instance*> my_list;
  for (Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr) {
    try {
      typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
      for (Iter iter = i->getRefAttr(*attr).begin(); iter != i->getRefAttr(*attr).end(); ++iter) {
        if ( (not ignore.exists(iter->first)) and (__bb.exists(iter->first)) ) {
          __eliminateNodesDownward(i, iter->first, pool, trash, elim_list, ignore);
        }
      }
    } catch (NotFound&) {
      // Ok
    }
  }
  // Eliminating all nodes in current instance
  if (hasEvidence(i)) {
    __eliminatNodesWithEvidence(i, pool, trash);
  } else {
    __insertLiftedNodes(i, pool, trash);
    for (Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg) {
      pool.insert(__getAggPotential(i, *agg));
    }
    try {
      InstanceBayesNet bn(*i);
      VariableElimination<prm_float> inf(bn);
      inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
    } catch (NotFound&) {
      // Raised if there is no inner nodes to eliminate
    }
  }
  // Calling elimination over child's parents
  while (not elim_list.empty()) {
    if (__checkElimOrder(i, elim_list.front())) {
      if ( (not ignore.exists(elim_list.front())) and (__bb.exists(elim_list.front())) ) {
        __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list, ignore);
      }
    } else if (__bb.exists(elim_list.front())) {
      ignore.insert(elim_list.front());
    }
    elim_list.popFront();
  }
  //__reduceElimList(i, my_list, elim_list, ignore, pool, trash);
  // Adding parents instance to elim_list
  for (Set<NodeId>::iterator sc = sc_set.begin(); sc != sc_set.end(); ++sc) {
    for (Set<Instance*>::iterator parent = i->getInstances(*sc).begin(); parent != i->getInstances(*sc).end(); ++parent) {
      if ((not ignore.exists(*parent)) and __bb.exists(*parent) and (*parent != from)) {
        elim_list.insert(*parent);
      }
    }
  }
}

void
SVED::__eliminateNodesUpward(const Instance* i,
                             BucketSet& pool, BucketSet& trash,
                             List<const Instance*>& elim_list,
                             Set<const Instance*>& ignore)
{
  ignore.insert(i);
  // Extracting required attributes and slotchains
  Set<NodeId>& attr_set = __getAttrSet(i);
  Set<NodeId>& sc_set   = __getSCSet(i);
  // Downward elimination
  for (Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr) {
    try {
      typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
      for (Iter iter = i->getRefAttr(*attr).begin(); iter != i->getRefAttr(*attr).end(); ++iter) {
        if ( (not ignore.exists(iter->first)) and (__bb.exists(iter->first)) ) {
          __eliminateNodesDownward(i, iter->first, pool, trash, elim_list, ignore);
        }
      }
    } catch (NotFound&) {
      // Ok
    }
  }
  // Eliminating all nodes in i instance
  if (hasEvidence(i)) {
    __eliminatNodesWithEvidence(i, pool, trash);
  } else {
    __insertLiftedNodes(i, pool, trash);
    for (Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg) {
      pool.insert(__getAggPotential(i, *agg));
    }
    try {
      InstanceBayesNet bn(*i);
      VariableElimination<prm_float> inf(bn);
      inf.eliminateNodes(__getElimOrder(i->type()), pool, trash);
    } catch (NotFound&) {
      // Raised if there is no inner nodes to eliminate
    }
  }
  // Eliminating instance in elim_list
  List<const Instance*> tmp_list;
  while (not elim_list.empty()) {
    if (__checkElimOrder(i, elim_list.front())) {
      if ( (not ignore.exists(elim_list.front())) and (__bb.exists(elim_list.front())) ) {
        __eliminateNodesDownward(i, elim_list.front(), pool, trash, elim_list, ignore);
      }
    } else if (__bb.exists(elim_list.front())) {
      ignore.insert(elim_list.front());
    }
    elim_list.popFront();
  }
  // Upward elimination
  for (Set<NodeId>::iterator sc = sc_set.begin(); sc != sc_set.end(); ++sc) {
    for (Set<Instance*>::iterator parent = i->getInstances(*sc).begin(); parent != i->getInstances(*sc).end(); ++parent) {
      if ( (not ignore.exists(*parent)) and (__bb.exists(*parent)) ) {
        __eliminateNodesUpward(*parent, pool, trash, tmp_list, ignore);
      }
    }
  }
}

void
SVED::__eliminatNodesWithEvidence(const Instance* i, BucketSet& pool, BucketSet& trash)
{
  // First we check if evidences are on inner nodes
  bool inner = false;
  for (EMapIterator e = evidence(i).begin(); e != evidence(i).end(); ++e) {
    inner = inner or i->type().isInputNode(i->get(e.key())) or i->type().isInnerNode(i->get(e.key()));
    if (inner) { break; }
  }
  if (inner) {
    BucketSet tmp_pool;
    __insertEvidence(i, tmp_pool);
    // We need a local to not eliminate queried inner nodes of the same class
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
SVED::__insertLiftedNodes(const Instance* i, BucketSet& pool, BucketSet& trash) {
  SVED::ArraySet* lifted_pool = 0;
  try {
    lifted_pool = __lifted_pools[&(__bb.requisiteNodes(i))];
  } catch (NotFound&) {
    __initLiftedNodes(i, trash);
    lifted_pool = __lifted_pools[&(__bb.requisiteNodes(i))];
  }
  for (SVED::ArraySetIterator iter = lifted_pool->begin(); iter != lifted_pool->end(); ++iter) {
    Potential<prm_float>* pot = new Potential<prm_float>( new MultiDimBijArray<prm_float>(i->bijection(), **iter));
    pool.insert(pot);
    trash.insert(pot);
  }
}

void
SVED::__initLiftedNodes(const Instance* i, BucketSet& trash)
{
  const Class& c = i->type();
  BucketSet lifted_pool;
  __lifted_pools.insert(&(__bb.requisiteNodes(i)), new SVED::ArraySet());
  for (Set<NodeId>::iterator node = __bb.requisiteNodes(i).begin(); node != __bb.requisiteNodes(i).end(); ++node) {
    if (c.get(*node).elt_type() == ClassElement::prm_attribute) {
      lifted_pool.insert(const_cast<Potential<prm_float>*>(&(c.get(*node).cpf())));
    }
  }
  ClassBayesNet bn(c);
  DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
  const std::vector<NodeId>& full_elim_order = t.eliminationOrder();
  VariableElimination<prm_float> inf(bn);
  // Removing Output nodes of elimination order
  std::vector<NodeId> inner_elim_order;
  std::vector<NodeId>* output_elim_order = new std::vector<NodeId>();
  for (size_t idx = 0; idx < full_elim_order.size(); ++idx) {
    if ((not c.isOutputNode(c.get(full_elim_order[idx]))) and (not ClassElement::isAggregate(c.get(full_elim_order[idx])))) {
      inner_elim_order.push_back(full_elim_order[idx]);
    } else {
      output_elim_order->push_back(full_elim_order[idx]);
    }
  }
  // If there is only output nodes and Aggregate we can't lift anything
  if (not inner_elim_order.empty()) {
    inf.eliminateNodes(inner_elim_order, lifted_pool, trash);
  }
  // Copying buckets in MultiDimArrays
  for(SVED::BucketSetIterator iter = lifted_pool.begin(); iter != lifted_pool.end(); ++iter) {
    const MultiDimBucket<prm_float>* b = 0;
    const MultiDimImplementation<prm_float>* impl = const_cast<const Potential<prm_float>&>((**iter)).getContent();
    b = dynamic_cast<const MultiDimBucket<prm_float>* >(impl);
    if (b != 0) {
      b->compute(true);
      try {
        __lifted_pools[&(__bb.requisiteNodes(i))]->insert(new MultiDimArray<prm_float>(b->bucket()));
      } catch (OperationNotAllowed&) {
        // Empty bucket
      }
    } else {
      const MultiDimArray<prm_float>* a = dynamic_cast<const MultiDimArray<prm_float>*>(impl);
      if (a != 0) {
        __lifted_pools[&(__bb.requisiteNodes(i))]->insert(new MultiDimArray<prm_float>(*a));
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
SVED::__initElimOrder() {
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
SVED::_marginal(const Chain& chain, Potential<prm_float>& m)
{
  const Instance* i = chain.first;
  const Attribute* elt = chain.second;
  SVED::BucketSet pool, trash;
  __bb.compute(i, elt->id());
  __eliminateNodes(i, elt->id(), pool, trash);
  m.fill((prm_float) 1);
  for (SVED::BucketSetIterator iter = pool.begin(); iter != pool.end(); ++iter) {
    if ((**iter).contains(*(m.variablesSequence().atPos(0)))) {
      m.multiplicateBy(**iter);
    }
  }
  m.normalize();
  // cleaning up the mess
  for (SVED::BucketSetIterator iter = trash.begin(); iter != trash.end(); ++iter) {
    delete *iter;
  }
  typedef HashTable<const Set<NodeId>*, SVED::ArraySet*>::iterator LiftedIter;
  for (LiftedIter iter = __lifted_pools.begin(); iter != __lifted_pools.end(); ++iter) {
    for (SVED::ArraySetIterator j = (**iter).begin(); j != (**iter).end(); ++j) {
      delete *j;
    }
    delete *iter;
  }
  __lifted_pools.clear();
  typedef HashTable< const Set<NodeId>*, std::pair<Set<NodeId>*, Set<NodeId>*> >::iterator
          ReqIter;
  for (ReqIter iter = __req_set.begin(); iter != __req_set.end(); ++iter) {
    delete iter->first;
    delete iter->second;
  }
  __req_set.clear();
  typedef HashTable<const Class*, std::vector<NodeId>*>::iterator ElimIter;
  for (ElimIter iter = __elim_orders.begin(); iter != __elim_orders.end(); ++iter) {
    delete *iter;
  }
  __elim_orders.clear();
}

void
SVED::_joint(const std::vector<Chain>& queries, Potential<prm_float>& j) {
  GUM_ERROR(FatalError, "Not implemented.");
}

void
SVED::__initReqSets(const Instance* i)
{
  Set<NodeId>* attr_set = new Set<NodeId>();
  Set<NodeId>* sc_set = new Set<NodeId>();
  for (Set<NodeId>::iterator iter = __bb.requisiteNodes(i).begin(); iter != __bb.requisiteNodes(i).end(); ++iter) {
    switch (i->type().get(*iter).elt_type()) {
      case ClassElement::prm_aggregate:
      case ClassElement::prm_attribute:
        {
          attr_set->insert(*iter);
          break;
        }
      case ClassElement::prm_slotchain:
        {
          sc_set->insert(*iter);
          break;
        }
      default:
        {
          GUM_ERROR(FatalError, "There should not be elements other"
              " than Attribute and SlotChain.");
        }
    }
  }
  __req_set.insert(&(__bb.requisiteNodes(i)), std::pair<Set<NodeId>*, Set<NodeId>*>(attr_set, sc_set));
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
