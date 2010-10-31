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
 * @brief Implementation of StructuredInference.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/structuredInference.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/structuredInference.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

StructuredInference::StructuredInference(const PRM& prm, const System& system):
  PRMInference(prm, system), __gspan(0), __dot(".")
{
  GUM_CONSTRUCTOR( StructuredInference );
  __gspan = new GSpan(prm, system);
}

StructuredInference::StructuredInference(const StructuredInference& source):
  PRMInference(source), __gspan(0), __dot(".")
{
  GUM_CONS_CPY( StructuredInference );
  __gspan = new GSpan(*_prm, *_sys);
}

StructuredInference::~StructuredInference() {
  GUM_DESTRUCTOR( StructuredInference );
  delete __gspan;
  typedef HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*>::iterator Iter;
  for (Iter iter = __elim_map.begin(); iter != __elim_map.end(); ++iter)
    delete (*iter);
  for (Set<Potential<prm_float>*>::iterator iter = __trash.begin();
       iter != __trash.end(); ++iter)
    delete (*iter);
}

StructuredInference&
StructuredInference::operator=(const StructuredInference& source) {
  _prm = source._prm;
  _sys = source._sys;
  delete __gspan;
  __gspan = new GSpan(*_prm, *_sys);
  return *this;
}

void
StructuredInference::_evidenceAdded(const Chain& chain) {

}

void
StructuredInference::_evidenceRemoved(const Chain& chain) {

}

void
StructuredInference::_marginal(const Chain& chain, Potential<prm_float>& m) {
  StructuredInference::RGData data;
  __buildReduceGraph(data);
  MultiDimBucket<prm_float> bucket;
  if (data.pool.size() > 1) {
    for (Set<Potential<prm_float>*>::iterator pot = data.pool.begin(); pot != data.pool.end(); ++pot) {
      if ((**pot).contains(__query.second->type().variable())) {
        bucket.add(*pot);
        GUM_ASSERT((**pot).variablesSequence().size() == 1);
      }
    }
    bucket.add(__query.second->type().variable());
    Instantiation i(bucket), j(m);
    for (i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc())
      m.set(j, bucket.get(i));
  } else {
    Potential<prm_float>* pot = *(data.pool.begin());
    GUM_ASSERT(pot->contains(__query.second->type().variable()));
    GUM_ASSERT(pot->variablesSequence().size() == 1);
    Instantiation i(*pot), j(m);
    for (i.setFirst(), j.setFirst(); not i.end(); i.inc(), j.inc())
      m.set(j, pot->get(i));
  }
}

void
StructuredInference::_joint(const std::vector< Chain >& queries, Potential<prm_float>& j) {
  GUM_ERROR(FatalError, "not implemented");
}

void
StructuredInference::__buildReduceGraph(StructuredInference::RGData& data) {
  // Reducing each used pattern
  typedef std::vector<gspan::Pattern*>::const_iterator Iter;
  for (Iter p = __gspan->patterns().begin(); p != __gspan->patterns().end();
       ++p)
    if (__gspan->matches(**p).size()) __reducePattern(*p);
  // Unreducing a matche if it contains the query
  if (__reducedInstances.exists(__query.first))
    __unreduceMatchWithQuery();
  // reducing instance not already reduced in a pattern
  __reduceAloneInstances(data);
  // Adding edges using the pools
  __addEdgesInReducedGraph(data);
  // Placing the query where it belongs
  NodeId id = data.var2node.second(&(__query.second->type().variable()));
  data.outputs().erase(id);
  data.queries().insert(id);
  // Triangulating, then eliminating
  PartialOrderedTriangulation t(&(data.reducedGraph), &(data.mods), &(data.partial_order));
  const std::vector<NodeId>& elim_order = t.eliminationOrder();
  for (size_t i = 0; i < data.outputs().size(); ++i)
    __eliminateNode(data.var2node.first(elim_order[i]), data.pool);
}

void
StructuredInference::__reducePattern(const gspan::Pattern* p) {
  // const gspan::InterfaceGraph& ig = __gspan->interfaceGraph();
  Set<Potential<prm_float>*> pool;
  StructuredInference::PData data(*p, __gspan->matches(*p));
  // First we add nodes to graph and fill mod, outputs, inners, vars, node2attr and pool
  __buildPatternGraph(data, pool);
  // Second we add observed nodes in all matches to obs and check for queries
  __buildObsSet(data);
  // Now we can triangulate graph
  PartialOrderedTriangulation t(&(data.graph), &(data.mod),
                                &(data.partial_order));
  const std::vector<NodeId>& elim_order = t.eliminationOrder();
  // We eliminate inner variables and then we eliminate observed nodes w.r.t.
  // each match's observation set
  for (size_t i = 0; i < data.inners().size(); ++i)
    __eliminateNode(data.vars.second(elim_order[i]), pool);
  for (GSpan::MatchedInstances::const_iterator iter = data.matches.begin();
       iter != data.matches.end(); ++iter)
    __elim_map.insert(*iter,
                      __eliminateObservedNodes(data, pool, **iter, elim_order));
}

void
StructuredInference::__buildPatternGraph(StructuredInference::PData& data,
                                         Set<Potential<prm_float>*>& pool)
{
  for (Sequence<Instance*>::const_iterator inst = data.match().begin();
       inst != data.match().end(); ++inst)
  {
    for (Instance::const_iterator attr = (**inst).begin();
         attr != (**inst).end(); ++attr)
    {
      // Adding the node
      NodeId id = data.graph.insertNode();
      data.node2attr.insert(id, __str(*inst, *attr));
      data.mod.insert(id, (**attr).type()->domainSize());
      data.vars.insert(id, &((**attr).type().variable()));
      pool.insert(const_cast<Potential<prm_float>*>(&((**attr).cpf())));
    }
  }
  // Second we add edges and nodes to inners or outputs
  for (Sequence<Instance*>::const_iterator inst = data.match().begin();
       inst != data.match().end(); ++inst)
  {
    for (Instance::const_iterator attr = (**inst).begin();
         attr != (**inst).end(); ++attr)
    {
      NodeId node = data.node2attr.first(__str(*inst, *attr));
      bool found = false; // If this is set at true, then node is an outer node
      // Children existing in the instance type's DAG
      const NodeSet& chldrn = (**inst).type().dag().children((**attr).id());
      for (NodeSetIterator chld = chldrn.begin(); chld != chldrn.end(); ++chld) {
        data.graph.insertEdge(node,
            data.node2attr.first(__str(*inst, (**inst).get(*chld))));
      }
      // Parents existing in the instance type's DAG
      const NodeSet& prnts  = (**inst).type().dag().parents((**attr).id());
      for (NodeSetIterator prnt = prnts.begin(); prnt != prnts.end(); ++prnt) {
        switch ((**inst).type().get(*prnt).elt_type()) {
          case ClassElement::prm_attribute:
          case ClassElement::prm_aggregate:
            {
              data.graph.insertEdge(node,
                  data.node2attr.first(__str(*inst, (**inst).get(*prnt))));
              break;
            }
          case ClassElement::prm_slotchain:
            {
              const Set<Instance*>& ref = (**inst).getInstances(*prnt);
              for (Set<Instance*>::const_iterator jnst = ref.begin();
                   jnst != ref.end(); ++jnst)
                if (data.match().exists(*jnst))
                  data.graph.insertEdge(node, data.node2attr.first( __str(*jnst, static_cast<const SlotChain&>((**inst).type().get(*prnt)))));
              break;
            }
          default: { /* Do nothing */ }
        }
      }
      // Referring Attribute
      const std::vector< std::pair<Instance*, std::string> >& ref_attr =
        (**inst).getRefAttr((**attr).id());
      typedef std::vector< std::pair<Instance*, std::string> >::const_iterator VecIter;
      for (VecIter pair = ref_attr.begin(); pair != ref_attr.end(); ++pair) {
        if (data.match().exists(pair->first)) {
          data.graph.insertEdge(node,
              data.node2attr.first(pair->first->name() + __dot + pair->second));
        } else {
          found = true;
        }
      }
      if (found)  data.outputs().insert(node);
      else        data.inners().insert(node);
    }
  }
}

void
StructuredInference::__buildObsSet(StructuredInference::PData& data)
{
  NodeId node = 0;
  for (GSpan::MatchedInstances::const_iterator iter = data.matches.begin();
       iter != data.matches.end(); ++iter)
  {
    for (Sequence<Instance*>::const_iterator jter = (**iter).begin();
         jter != (**iter).end(); ++jter)
    {
      if (hasEvidence(*jter)) {
        for (PRMInference::EMap::const_iterator kter = evidence(*jter).begin();
             kter != evidence(*jter).end(); ++kter)
        {
          try {
            std::string name = data.match().atPos(jter.pos())->name() + __dot + (**jter).get(kter.key()).safeName();
            node = data.node2attr.first(name);
            data.obs().insert(node);
            data.inners().erase(node);
          } catch (DuplicateElement&) {
            // Do nothing
          }
        }
      }
    }
  }
}

Set<Potential<prm_float>*>*
StructuredInference::__eliminateObservedNodes(StructuredInference::PData& data,
                                        const Set<Potential<prm_float>*>& pool,
                                        const Sequence<Instance*>& match,
                                        const std::vector<NodeId>& elim_order)
{
  Set<Potential<prm_float>*> set(pool);
  const PRMInference::EMap* e_map = 0;
  Potential<prm_float>* pot = 0;
  Bijection<const gum::DiscreteVariable*, const gum::DiscreteVariable*> bij;
  for (Sequence<Instance*>::const_iterator iter = match.begin();
       iter != match.end(); ++iter)
  {
    if (hasEvidence(*iter)) {
      e_map = &(evidence(*iter));
      for (PRMInference::EMap::const_iterator obs = e_map->begin();
           obs != e_map->end(); ++obs)
      {
        // We do not need to check the content of the MultiDim,
        // since PRMInference garantees a MultiDimArray
        bij.insert(&((**iter).get(obs.key()).type().variable()),
                   &(match.atPos(iter.pos())->get(obs.key()).type().variable()));
        pot = new Potential<prm_float>(
            new MultiDimBijArray<prm_float>(bij,
              static_cast<const MultiDimArray<prm_float>&>(*((**obs).getContent()))));
        set.insert(pot);
        __trash.insert(pot);
        set.insert(pot);
        __trash.insert(pot);
        bij.eraseFirst(&((**iter).get(obs.key()).type().variable()));
      }
    }
    __reducedInstances.insert(*iter);
  }
  for (size_t idx = data.inners().size(); idx < data.obs().size(); ++idx)
    __eliminateNode(data.vars.second(elim_order[idx]), set);
  if (match.exists(const_cast<Instance*>(__query.first))) {
    // Maybe this test costs more than doing an unnecessary translation (I doubt it,
    // but this should be tested).
    __pdata = new StructuredInference::PData(data);
    return 0;
  } else {
    return __translatePotSet(set, data.match(), match);
  }
}

Set<Potential<prm_float>*>*
StructuredInference::__translatePotSet(Set<Potential<prm_float>*>& set,
                                       const Sequence<Instance*>& source,
                                       const Sequence<Instance*>& match)
{
  Set<Potential<prm_float>*>* retVal = new Set<Potential<prm_float>*>();
  Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;
  const Set<Instance*>* refs = 0;
  const Set<Instance*>* tefs = 0;
  typedef std::vector< std::pair< Instance*, std::string> >::const_iterator Iter;
  const std::vector< std::pair<Instance*, std::string> >* v = 0;
  Size size = 0;
  // We fill the bijection, then we translate the potentials
  for (Sequence<Instance*>::const_iterator iter = source.begin();
      iter != source.end(); ++iter)
  {
    // First we translate each attribute pointed by a SlotChain
    for (Set<SlotChain*>::const_iterator sc =
        (**iter).type().slotChains().begin();
        sc != (**iter).type().slotChains().end(); ++sc)
    {
      if ((**sc).isMultiple()) {
        refs = &((**iter).getInstances((**sc).id()));
        tefs = &(match.atPos(iter.pos())->getInstances((**sc).id()));
        Set<Instance*>::const_iterator ref = refs->begin();
        Set<Instance*>::const_iterator tef = tefs->begin();
        GUM_ASSERT(refs->size() == tefs->size());
        for (; ref != refs->end(); ++ref, ++tef) {
          bij.insert(&((**ref).get((**sc).lastElt().safeName()).type().variable()),
              &((**tef).get((**sc).lastElt().safeName()).type().variable()));
        }
      } else {
        bij.insert(&((**iter).getInstance((**sc).id()).get((**sc).lastElt().safeName()).type().variable()),
            &(match.atPos(iter.pos())->getInstance((**sc).id()).get((**sc).lastElt().safeName()).type().variable()));
      }
    }
    // Second we translate referred attributes
    for (Instance::InvRefConstIterator inv = (**iter).beginInvRef();
        inv != (**iter).endInvRef(); ++inv)
    {
      for (Iter pair = (**inv).begin(); pair != (**inv).end(); ++pair) {
        // Checking the existence of the refered instance in the match set
        // source
        if (not source.exists(pair->first)) {
          size = bij.size();
          v = &(match.atPos(iter.pos())->getRefAttr(inv.key()));
          for (Iter mair = v->begin(); mair != v->end(); ++mair) {
            // Where looking for a not yet added refered instance, maybe not
            // optimal but suppose very few things about the refered instance
            // ordering
            if ( (mair->first->type() == pair->first->type()) and
                (not match.exists(mair->first)) )
            {
              try {
                bij.insert(&(pair->first->get(pair->second).type().variable()),
                    &(mair->first->get(mair->second).type().variable()));
                break;
              } catch (DuplicateElement&) {
                // Lets search for the next possible occurrence
              }
            }
          }
          GUM_ASSERT(size == bij.size() + 1);
        }
      }
    }
  }
  // Third we translate the potentials
  for (Set<Potential<prm_float>*>::const_iterator iter = set.begin();
      iter != set.end(); ++iter)
    retVal->insert(copyPotential(bij, **iter));
  return retVal;
}

void
StructuredInference::__unreduceMatchWithQuery() {
  GSpan::MatchedInstances& matches = __gspan->matches(__pdata->pattern);
  // First step is to find the match
  for (GSpan::MatchedInstances::iterator iter = matches.begin();
       iter != matches.end(); ++iter)
  {
    if ((**iter).exists(const_cast<Instance*>(__query.first))) {
      Sequence<Instance*>* match = *iter;
      std::string name = __query.first->name() + __dot + __query.second->safeName();
      NodeId node = __pdata->node2attr.first(name);
      __pdata->inners().erase(node);
      __pdata->outputs().erase(node);
      __pdata->queries().insert(node);
      Set<Potential<prm_float>*>* pool = new Set<Potential<prm_float>*>();
      for (Sequence<Instance*>::iterator i = match->begin(); i != match->end(); ++i)
        for (Instance::iterator a = (**i).begin(); a != (**i).end(); ++i)
          pool->insert(&(**a).cpf());
      PartialOrderedTriangulation t(&(__pdata->graph), &(__pdata->mod), &(__pdata->partial_order));
      size_t size = __pdata->inners().size() + __pdata->obs().size();
      for (size_t idx = 0; idx < size; ++idx)
        __eliminateNode(__pdata->vars.second(t.eliminationOrder()[idx]), *pool);
      __elim_map[match] = pool;
      return;
    }
  }
  GUM_ERROR(FatalError, "__unreduceMatchWithQuery() "
                        "was called but no matches contained the queried instance");
}

void
StructuredInference::__reduceAloneInstances(StructuredInference::RGData& rg_data) {
  StructuredInference::CData* data = 0;
  Potential<prm_float>* pot = 0;
  for (System::const_iterator i = _sys->begin(); i != _sys->end(); ++i) {
    if (not __reducedInstances.exists(*i)) {
      Set<Potential<prm_float>*> pool;
      try {
        data = __cdata_map[&((**i).type())];
      } catch (NotFound&) {
        data = new StructuredInference::CData((**i).type());
        __cdata_map.insert(&((**i).type()), data);
        data->instances.insert(*i);
      }
      if (__query.first == *i) {
        List<NodeSet> partial_order;
        partial_order.insert(data->inners());
        for (NodeSet::iterator node = data->aggregators().begin(); node != data->aggregators().end(); ++node)
          partial_order[0].insert(*node);
        partial_order.insert(data->outputs());
        partial_order.insert(NodeSet());
        partial_order[0].erase(__query.second->id());
        partial_order[1].erase(__query.second->id());
        partial_order[2].insert(__query.second->id());
        for (PRMInference::EMap::iterator e = evidence(*i).begin(); e != evidence(*i).end(); ++e)
          pool.insert(const_cast<Potential<prm_float>*>(*e));
        PartialOrderedTriangulation t(&(data->moral_graph), &(data->mods), &(partial_order));
        for (size_t idx = 0; idx < partial_order[0].size(); ++idx)
          __eliminateNode(&((**i).get(t.eliminationOrder()[idx]).type().variable()), pool);
      } else if (hasEvidence(*i)) {
        List<NodeSet> partial_order;
        partial_order.insert(data->inners());
        for (NodeSet::iterator node = data->aggregators().begin(); node != data->aggregators().end(); ++node)
          partial_order[0].insert(*node);
        partial_order.insert(data->outputs());
        for (PRMInference::EMap::iterator e = evidence(*i).begin(); e != evidence(*i).end(); ++e)
          pool.insert(const_cast<Potential<prm_float>*>(*e));
        PartialOrderedTriangulation t(&(data->moral_graph), &(data->mods), &(partial_order));
        for (size_t idx = 0; idx < partial_order[0].size(); ++idx)
          __eliminateNode(&((**i).get(t.eliminationOrder()[idx]).type().variable()), pool);
      } else {
        // We eliminate the class inner nodes if it's not yet done
        if (not data->pool.empty())
          for (size_t i = 0; i < data->inners().size(); ++i)
            __eliminateNode(&(data->c.get(data->elim_order()[i]).type().variable()), data->pool);
        // We translate the class level potentials into the instance ones and
        // proceed with elimination
        for (Set<Potential<prm_float>*>::iterator iter = data->pool.begin(); iter != data->pool.end(); ++iter) {
          pot = copyPotential((**i).bijection(), **iter);
          pool.insert(pot);
          __trash.insert(pot);
        }
        for (Set<Aggregate*>::const_iterator agg = data->c.aggregates().begin(); agg != data->c.aggregates().end(); ++agg)
          pool.insert(&((**i).get((**agg).id()).cpf()));
        // We eliminate inner aggregators with their parents if necessary (see CData constructor)
        Size size = data->inners().size() + data->aggregators().size();
        for (size_t idx = data->inners().size(); idx < size; ++idx)
          __eliminateNode(&((**i).get(data->elim_order()[idx]).type().variable()), pool);
      }
      // Now, we simply add the remaining potentials to the global pool and to the reduce graph
      typedef Sequence<const DiscreteVariable*>::const_iterator VarIter;
      const Sequence<const DiscreteVariable*>* vars = 0;
      NodeId id = 0;
      for (Set<Potential<prm_float>*>::iterator iter = pool.begin(); iter != pool.end(); ++iter) {
        rg_data.pool.insert(*iter);
        vars = &((**iter).variablesSequence());
        for (VarIter var = vars->begin(); var != vars->end(); ++var) {
          if (not rg_data.var2node.existsFirst(*var)) {
            id = rg_data.reducedGraph.insertNode();
            rg_data.var2node.insert(*var, id);
            rg_data.mods.insert(id, (**var).domainSize());
            rg_data.outputs().insert(id);
          }
        }
      }
    }
  }
}

void
StructuredInference::__addEdgesInReducedGraph(StructuredInference::RGData& data)
{
  // We first add edges between variables already in pool (i.e. those of the reduced instances)
  const Sequence<const DiscreteVariable*>* vars = 0;
  for (Set<Potential<prm_float>*>::iterator pot = data.pool.begin(); pot != data.pool.end(); ++pot) {
    vars = &((**pot).variablesSequence());
    for (Size var_1 = 0; var_1 < vars->size(); ++var_1)
      for (Size var_2 = var_1 + 1; var_2 < vars->size(); ++var_2)
        data.reducedGraph.insertEdge(data.var2node.second(vars->atPos(var_1)),
                                     data.var2node.second(vars->atPos(var_2)));
  }
  // Adding potentials obtained from reduced patterns
  typedef HashTable<const Sequence<Instance*>*, Set<Potential<prm_float>*>*>::iterator Jter;
  Set<Potential<prm_float>*>* pool = 0;
  Potential<prm_float>* pot = 0;
  NodeId id = 0;
  for (Jter iter = __elim_map.begin(); iter != __elim_map.end(); ++iter) {
    pool = *iter;
    // First we add edges between variables in the same reduced patterns
    for (Set<Potential<prm_float>*>::iterator jter = pool->begin(); jter != pool->end(); ++jter) {
      pot = *jter;
      vars = &(pot->variablesSequence());
      for (Size var_1 = 0; var_1 < vars->size(); ++var_1) {
        for (Size var_2 = var_1 + 1; var_2 < vars->size(); ++var_2) {
          try {
            data.reducedGraph.insertEdge(data.var2node.second(vars->atPos(var_1)),
                                         data.var2node.second(vars->atPos(var_2)));
          } catch (NotFound&) {
            // We need to add nodes !
            if (not data.var2node.existsFirst(vars->atPos(var_1))) {
              id = data.reducedGraph.insertNode();
              data.var2node.insert(vars->atPos(var_1), id);
              data.mods.insert(id, vars->atPos(var_1)->domainSize());
              data.outputs().insert(id);
            }
            if (not data.var2node.existsFirst(vars->atPos(var_2))) {
              id = data.reducedGraph.insertNode();
              data.var2node.insert(vars->atPos(var_2), id);
              data.mods.insert(id, vars->atPos(var_2)->domainSize());
              data.outputs().insert(id);
            }
            data.reducedGraph.insertEdge(data.var2node.second(vars->atPos(var_1)),
                                         data.var2node.second(vars->atPos(var_2)));
          }
        }
      }
      data.pool.insert(pot);
    }
  }
}

void
StructuredInference::__eliminateNode(const DiscreteVariable* var,
                                     Set<Potential<prm_float>*>& pool)
{
  MultiDimBucket<prm_float>* bucket = new MultiDimBucket<prm_float>();
  Set< Potential<prm_float>* > toRemove;
  for (SetIterator<Potential<prm_float>*> iter = pool.begin();
       iter != pool.end(); ++iter )
  {
    if ((*iter)->contains(*var)) {
      bucket->add(**iter);
      toRemove.insert(*iter);
    }
  }
  if (toRemove.empty()) {
    delete bucket;
  } else {
    for (SetIterator<Potential<prm_float>*> iter = toRemove.begin();
         iter != toRemove.end(); ++iter)
      pool.erase( *iter );
    for (Set<const DiscreteVariable*>::iterator jter =
         bucket->allVariables().begin(); jter != bucket->allVariables().end();
         ++jter )
    {
      try {
        if ((*jter) != var) bucket->add( **jter );
      } catch (NotFound&) {
        // This can happen if since some DiscreteVariable are not represented
        // as nodes in the undigraph (parents of input nodes)
        bucket->add(**jter);
      }
    }
    Potential<prm_float>* bucket_pot = new Potential<prm_float>( bucket );
    __trash.insert( bucket_pot );
    pool.insert( bucket_pot );
  }
}

StructuredInference::RGData::RGData() {
  GUM_CONSTRUCTOR(StructuredInference::RGData);
  partial_order.insert(NodeSet());
  partial_order.insert(NodeSet());
}

StructuredInference::PData::PData(const gspan::Pattern& p,
                                  const GSpan::MatchedInstances& m):
  pattern(p), matches(m)
{
  GUM_CONSTRUCTOR(StructuredInference::PData);
  for (int i = 0; i < 4; ++i)
    partial_order.push_front(NodeSet());
}

StructuredInference::PData::PData(const StructuredInference::PData& source):
  pattern(source.pattern), matches(source.matches), graph(source.graph),
  mod(source.mod), partial_order(source.partial_order), node2attr(source.node2attr),
  vars(source.vars)
{
  GUM_CONS_CPY(StructuredInference::PData);
}

StructuredInference::CData::CData(const Class& a_class):
  c(c), __elim_order(0)
{
  GUM_CONSTRUCTOR(StructuredInference::CData);
  partial_order.insert(NodeSet());
  partial_order.insert(NodeSet());
  partial_order.insert(NodeSet());
  const NodeSet* parents = 0;
  for (DAG::NodeIterator node = c.dag().beginNodes();
       node != c.dag().endNodes(); ++node)
  {
    switch (c.get(*node).elt_type()) {
      case ClassElement::prm_aggregate:
        {
          if (c.isOutputNode(c.get(*node)))
            partial_order[2].insert(*node);
          else
            partial_order[1].insert(*node);
          // If the aggregators is an input and have parents which are not
          // outputs, we must eliminate the parents after adding the
          // aggregator's CPT
          parents =  &(c.dag().parents(*node));
          for (NodeSet::const_iterator prnt = parents->begin();
               prnt != parents->end(); ++prnt)
          {
            if (not c.isOutputNode(c.get(*prnt))) {
              partial_order[0].erase(*prnt);
              partial_order[1].insert(*prnt);
            }
          }
          break;
        }
      case ClassElement::prm_attribute:
        {
          moral_graph.insertNode(*node);
          pool.insert(const_cast<Potential<prm_float>*>(&(c.get(*node).cpf())));
          if (c.isOutputNode(c.get(*node)))
            partial_order[2].insert(*node);
          else if (not partial_order[1].exists(*node))
            partial_order[0].insert(*node);
          break;
        }
      default: { /* Do nothing */ }
    }
  }
  // We moralise the graph
  for (DAG::ArcIterator iter = c.dag().beginArcs(); iter != c.dag().endArcs();
       ++iter)
    moral_graph.insertEdge(iter->first(), iter->second());
  for (DAG::NodeIterator iter = c.dag().beginNodes();
       iter != c.dag().endNodes(); ++iter)
  {
    parents = &(c.dag().parents(*iter));
    for (NodeSetIterator it1 = parents->begin(); it1 != parents->end(); ++it1) {
      NodeSetIterator it2 = it1;
      for (++it2; it2 != parents->end(); ++it2)
        moral_graph.insertEdge(*it1, *it2);
    }
  }
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
