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
  PRMInference(prm, system), __gspan(0)
{
  GUM_CONSTRUCTOR( StructuredInference );
  __gspan = new GSpan(prm, system);
}

StructuredInference::StructuredInference(const StructuredInference& source):
  PRMInference(source), __gspan(0)
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
  for (Set<Potential<prm_float>*>::iterator iter = __trash.begin(); iter != __trash.end(); ++iter)
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
  // __q.clear();
  // __q.push_back(chain);
}

void
StructuredInference::_joint(const std::vector< Chain >& queries, Potential<prm_float>& j) {
  // __q.clear();
  // __q = queries;
}

void
StructuredInference::__reducePattern(const gspan::Pattern* p) {
  std::string dot = ".";
  // const gspan::InterfaceGraph& ig = __gspan->interfaceGraph();
  Set<Potential<prm_float>*> pool;
  // We'll use the first match for computations
  const GSpan::MatchedInstances& matches = __gspan->matches(*p);
  const Sequence<Instance*>& match = **(matches.begin());
  // We'll use a PartialOrderedTriangulation with three sets: output, nodes and obs
  // with children outside the pattern and the other nodes
  List<NodeSet> partial_order;
  for (int i = 0; i < 4; ++i) partial_order.push_front(NodeSet());
  NodeSet& inners = partial_order[0];
  NodeSet& obs = partial_order[1];
  NodeSet& outputs = partial_order[2];
  //NodeSet& queries = partial_order[3];
  UndiGraph* graph = new UndiGraph();   // A yet to be triangulated undigraph
  Property<unsigned int>::onNodes mod;  // The undigraph node's modalities
  Bijection<NodeId, const DiscreteVariable*> vars;
  // Finally we declared a bijection to easily keep track  bettween graph and attributes
  Bijection<NodeId, std::string> node2attr; // its of the form instance_name DOT attr_name
  // First we add nodes to graph and fill set and mod
  __buildPatternGraph(*graph, match, outputs, inners, mod, vars, node2attr, pool);
  // Second we add observed nodes in all matches to obs
  __buildObsSet(matches, match, obs, node2attr);
  // Now we can triangulate graph
  PartialOrderedTriangulation t(graph, &mod, &partial_order);
  const std::vector<NodeId>& elim_order = t.eliminationOrder();
  // We eliminate inner variables and then we eliminate observed nodes w.r.t. each match observation set
  for (size_t i = 0; i < inners.size(); ++i)
    __eliminateNode(vars.second(elim_order[i]), pool);
  for (GSpan::MatchedInstances::const_iterator iter = matches.begin(); iter != matches.end(); ++iter)
    __elim_map.insert(*iter, __eliminateObservedNodes(vars, pool, match, **iter, elim_order, inners.size(), obs.size()));
}

void
StructuredInference::__buildPatternGraph(UndiGraph& graph, const Sequence<Instance*>& match,
                                         NodeSet& outputs, NodeSet& inners,
                                         Property<unsigned int>::onNodes& mod,
                                         Bijection<NodeId, const DiscreteVariable*>& vars,
                                         Bijection<NodeId, std::string>& node2attr,
                                         Set<Potential<prm_float>*>& set)
{
  std::string dot = ".";
  for (Sequence<Instance*>::const_iterator inst = match.begin(); inst != match.end(); ++inst) {
    for (Instance::const_iterator attr = (**inst).begin(); attr != (**inst).end(); ++attr) {
      std::string name = (**inst).name() + dot + (**attr).safeName();
      // Adding the node
      NodeId id = graph.insertNode();
      node2attr.insert(id, name);
      mod.insert(id, (**attr).type()->domainSize());
      vars.insert(id, &((**attr).type().variable()));
      set.insert(const_cast<Potential<prm_float>*>(&((**attr).cpf())));
    }
  }
  // Second we add edges and nodes to inners or outputs
  for (Sequence<Instance*>::const_iterator inst = match.begin(); inst != match.end(); ++inst) {
    for (Instance::const_iterator attr = (**inst).begin(); attr != (**inst).end(); ++attr) {
      NodeId node = node2attr.first((**inst).name() + dot + (**attr).safeName());
      std::string tail, head;
      bool found = false; // If this is set at true, then node is an outer node
      // Children existing in the instance type's DAG
      const NodeSet& chldrn = (**inst).type().dag().children((**attr).id());
      for (NodeSetIterator chld = chldrn.begin(); chld != chldrn.end(); ++chld) {
        head = (**inst).name() + dot + (**inst).get(*chld).safeName();
        graph.insertEdge(node, node2attr.first(head));
      }
      // Parents existing in the instance type's DAG
      const NodeSet& prnts  = (**inst).type().dag().parents((**attr).id());
      for (NodeSetIterator prnt = prnts.begin(); prnt != prnts.end(); ++prnt) {
        switch ((**inst).type().get(*prnt).elt_type()) {
          case ClassElement::prm_attribute:
          case ClassElement::prm_aggregate:
            {
              tail = (**inst).name() + dot + (**inst).get(*prnt).safeName();
              graph.insertEdge(node, node2attr.first(tail));
              break;
            }
          case ClassElement::prm_slotchain:
            {
              const Set<Instance*>& ref = (**inst).getInstances(*prnt);
              for (Set<Instance*>::const_iterator jnst = ref.begin(); jnst != ref.end(); ++jnst) {
                if (match.exists(*jnst)) {
                  tail = (**jnst).name() + dot + static_cast<const SlotChain&>((**inst).type().get(*prnt)).lastElt().safeName();
                  graph.insertEdge(node, node2attr.first(tail));
                } else {
                  found = true;
                }
              }
              break;
            }
          default: { /* Do nothing */ }
        }
      }
      // Referring Attribute
      const std::vector< std::pair<Instance*, std::string> >& ref_attr = (**inst).getRefAttr((**attr).id());
      typedef std::vector< std::pair<Instance*, std::string> >::const_iterator VecIter;
      for (VecIter pair = ref_attr.begin(); pair != ref_attr.end(); ++pair) {
        if (match.exists(pair->first)) {
          head = pair->first->name() + dot + pair->second;
          graph.insertEdge(node, node2attr.first(head));
        } else {
          found = true;
        }
      }
      if (found)  outputs.insert(node);
      else        inners.insert(node);
    }
  }
}

void
StructuredInference::__buildObsSet(const GSpan::MatchedInstances& matches, const Sequence<Instance*>& match, NodeSet& obs,
                                   Bijection<NodeId, std::string>& node2attr)
{
  std::string dot = ".";
  for (GSpan::MatchedInstances::const_iterator iter = matches.begin(); iter != matches.end(); ++iter) {
    for (Sequence<Instance*>::const_iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
      if (hasEvidence(*jter)) {
        for (PRMInference::EMap::const_iterator node = evidence(*jter).begin(); node != evidence(*jter).end(); ++node) {
          try {
            obs.insert(node2attr.first(match.atPos(jter.pos())->name() + dot + (**jter).get(node.key()).safeName()));
          } catch (DuplicateElement&) {
            // Do nothing
          }
        }
      }
    }
  }
}

void
StructuredInference::__eliminateNode(const DiscreteVariable* var,
                                     Set<Potential<prm_float>*>& pool)
{
  MultiDimBucket<prm_float>* bucket = new MultiDimBucket<prm_float>();
  Set< Potential<prm_float>* > toRemove;
  for ( SetIterator<Potential<prm_float>*> iter = pool.begin(); iter != pool.end(); ++iter ) {
    if ((*iter)->contains(*var)) {
      bucket->add(**iter);
      toRemove.insert(*iter);
    }
  }
  if (toRemove.empty()) {
    delete bucket;
  } else {
    for (SetIterator<Potential<prm_float>*> iter = toRemove.begin(); iter != toRemove.end(); ++iter)
      pool.erase( *iter );
    for (Set<const DiscreteVariable*>::iterator jter = bucket->allVariables().begin(); jter != bucket->allVariables().end(); ++jter ) {
      try {
        if ((*jter) != var) bucket->add( **jter );
      } catch (NotFound&) {
        // This can happen if since some DiscreteVariable are not represented as nodes in the
        // undigraph (parents of input nodes)
        bucket->add(**jter);
      }
    }
    Potential<prm_float>* bucket_pot = new Potential<prm_float>( bucket );
    __trash.insert( bucket_pot );
    pool.insert( bucket_pot );
  }
}

Set<Potential<prm_float>*>*
StructuredInference::__eliminateObservedNodes(const Bijection<NodeId, const DiscreteVariable*>& vars,
                                              const Set<Potential<prm_float>*>& pool,
                                              const Sequence<Instance*>& source,
                                              const Sequence<Instance*>& match,
                                              const std::vector<NodeId>& elim_order,
                                              Size start, Size end)
{
  Set<Potential<prm_float>*> set(pool);
  for (Sequence<Instance*>::const_iterator iter = match.begin(); iter != match.end(); ++iter) {
    if (hasEvidence(*iter)) {
      const PRMInference::EMap& e_map = evidence(*iter);
      for (PRMInference::EMap::const_iterator obs = e_map.begin(); obs != e_map.end(); ++obs) {
        Potential<prm_float>* pot = 0;
        if (dynamic_cast<const MultiDimArray<prm_float>*>((**obs).getContent())) {
          Bijection<const gum::DiscreteVariable*, const gum::DiscreteVariable*> bij;
          bij.insert(&((**iter).get(obs.key()).type().variable()), &( match.atPos(iter.pos())->get(obs.key()).type().variable()));
          pot = new Potential<prm_float>(new MultiDimBijArray<prm_float>(bij, static_cast<const MultiDimArray<prm_float>&>(*((**obs).getContent()))));
          set.insert(pot);
          __trash.insert(pot);
        } else {
          pot = new Potential<prm_float>();
          pot->add((**iter).get(obs.key()).type().variable());
          Instantiation inst(*pot), jnst(**obs);
          for (inst.setFirst(), jnst.setFirst(); not inst.end(); inst.inc(), jnst.inc())
            pot->set(inst, (**obs).get(jnst));
        }
        set.insert(pot);
        __trash.insert(pot);
      }
    }
  }
  for (size_t idx = start; idx < end; ++idx)
    __eliminateNode(vars.second(elim_order[idx]), set);
  return __translatePotSet(set, source, match);
}

Set<Potential<prm_float>*>*
StructuredInference::__translatePotSet(Set<Potential<prm_float>*>& set,
                                       const Sequence<Instance*>& source,
                                       const Sequence<Instance*>& match)
{
  Set<Potential<prm_float>*>* retVal = new Set<Potential<prm_float>*>();
  Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;
  for (Sequence<Instance*>::const_iterator iter = source.begin(); iter != source.end(); ++iter) {
    // First we translate each attribute pointed by a SlotChain
    for (Set<SlotChain*>::const_iterator sc = (**iter).type().slotChains().begin(); sc != (**iter).type().slotChains().end(); ++sc) {
      if ((**sc).isMultiple()) {
        const Set<Instance*>& refs = (**iter).getInstances((**sc).id());
        Set<Instance*>::const_iterator ref = refs.begin();
        const Set<Instance*>& tefs = match.atPos(iter.pos())->getInstances((**sc).id());
        Set<Instance*>::const_iterator tef = tefs.begin();
        GUM_ASSERT(refs.size() == tefs.size());
        for (; ref != refs.end(); ++ref, ++tef)
          bij.insert(&((**ref).get((**sc).lastElt().safeName()).type().variable()),
              &((**tef).get((**sc).lastElt().safeName()).type().variable()));
      } else {
        bij.insert(&((**iter).getInstance((**sc).id()).get((**sc).lastElt().safeName()).type().variable()),
                   &(match.atPos(iter.pos())->getInstance((**sc).id()).get((**sc).lastElt().safeName()).type().variable()));
      }
    }
    // Second we translate referred attributes
    for (Instance::InvRefConstIterator inv = (**iter).beginInvRef(); inv != (**iter).endInvRef(); ++inv) {
      typedef std::vector< std::pair< Instance*, std::string> >::const_iterator Iter;
      const std::vector< std::pair<Instance*, std::string> >* v = 0;
      for (Iter pair = (**inv).begin(); pair != (**inv).end(); ++pair) {
        // Checking the existence of the refered instance in the match set source
        if (not source.exists(pair->first)) {
          Size size = bij.size();
          v = &(match.atPos(iter.pos())->getRefAttr(inv.key()));
          for (Iter mair = v->begin(); mair != v->end(); ++mair) {
            // Where looking for a not yet added refered instance, maybe not optimal but suppose
            // very few things about the refered instance ordering
            if ( (mair->first->type() == pair->first->type()) and (not match.exists(mair->first)) ) {
              try {
                bij.insert(&(pair->first->get(pair->second).type().variable()),
                           &(mair->first->get(mair->second).type().variable()));
                break;
              } catch (DuplicateElement&) {
                // Lets search for the next possible occurrence
              }
            }
          }
          if (size != (bij.size() + 1)) {
            GUM_ERROR(NotFound, "could not translate a referring attribute");
          }
        }
      }
    }
  }
  // Third we translate the potentials
  for (Set<Potential<prm_float>*>::const_iterator iter = set.begin(); iter != set.end(); ++iter) {
    if (dynamic_cast<const MultiDimArray<prm_float>*>((**iter).getContent())) {
      retVal->insert(new Potential<prm_float>(new MultiDimBijArray<prm_float>(bij,
                                              *(static_cast<const MultiDimArray<prm_float>*>((**iter).getContent())))));
    } else if (dynamic_cast<const MultiDimBucket<prm_float>*>((**iter).getContent())) {
      retVal->insert(new Potential<prm_float>(new MultiDimBijArray<prm_float>(bij,
                                              static_cast<const MultiDimBucket<prm_float>*>((**iter).getContent())->bucket())));
    } else {
      MultiDimArray<prm_float>* array = new MultiDimArray<prm_float>();
      for (Sequence<const DiscreteVariable*>::const_iterator var = (**iter).variablesSequence().begin(); var != (**iter).variablesSequence().end(); ++var)
        array->add(*(bij.second(*var)));
      Instantiation inst(**iter), jnst(*array);
      for (inst.setFirst(), jnst.setFirst(); not inst.end(); inst.inc(), jnst.inc())
        array->set(jnst, (**iter).get(inst));
      retVal->insert(new Potential<prm_float>(array));
    }
  }
  return retVal;
}

// void
// StructuredInference::__translateInnerNodesElimination(const Set<Potential<prm_float>*>& pool,
//                                                       const Set<Potential<prm_float>*>& trash,
//                                                       const Sequence<Instance*>& source,
//                                                       const Sequence<Instance*>& match)
// {
// 
// }

} /* namespace prm */
} /* namespace gum */
// ============================================================================
