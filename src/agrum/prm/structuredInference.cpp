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

}

void
StructuredInference::_joint(const std::vector< Chain >& queries, Potential<prm_float>& j) {

}

void
StructuredInference::__reducePattern(const gspan::Pattern* p) {
  std::string dot = ".";
  // const gspan::InterfaceGraph& ig = __gspan->interfaceGraph();
  Set<Potential<prm_float>*> pool;
  Set<Potential<prm_float>*> trash;
  // We'll use the first match for computations
  const GSpan::MatchedInstances& matches = __gspan->matches(*p);
  const Sequence<Instance*>& match = **(matches.begin());
  // We'll use a PartialOrderedTriangulation with three sets: output, nodes and obs
  // with children outside the pattern and the other nodes
  List<NodeSet> partial_order;
  for (int i = 0; i < 3; ++i) partial_order.push_front(NodeSet());
  NodeSet& inners = partial_order[0];
  NodeSet& obs = partial_order[1];
  NodeSet& outputs = partial_order[2];
  UndiGraph* graph = new UndiGraph();   // A yet to be triangulated undigraph
  Property<unsigned int>::onNodes mod;  // The undigraph node's modalities
  Bijection<NodeId, const DiscreteVariable*> vars;
  // Finally we declared a bijection to easily keep track  bettween graph and attributes
  Bijection<NodeId, std::string> node2attr; // its of the form instance_name DOT attr_name
  // First we add nodes to graph and fill set, mod
  __buildPatternGraph(*graph, match, outputs, inners, mod, vars, node2attr, pool);
  // Second we add observed nodes in all matches to obs
  __buildObsSet(matches, match, obs, node2attr);
  // Now we can triangulate graph
  PartialOrderedTriangulation t(graph, &mod, &partial_order);
  const std::vector<NodeId>& elim_order = t.eliminationOrder();
  // We eliminate inner variables and we translate the variable for each match
  for (size_t i = 0; i < inners.size(); ++i)
    __eliminateNode(vars.second(elim_order[i]), pool, trash );

}

void
StructuredInference::__buildPatternGraph(UndiGraph& graph, const Sequence<Instance*>& match,
                                         NodeSet& outputs, NodeSet& inners,
                                         Property<unsigned int>::onNodes& mod,
                                         Bijection<NodeId, const DiscreteVariable*>& vars,
                                         Bijection<NodeId, std::string>& node2attr,
                                         StructuredInference::ReduceSet& set)
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
      const ArcSet& chldrn = (**inst).type().dag().children((**attr).id());
      for (ArcSet::const_iterator chld = chldrn.begin(); chld != chldrn.end(); ++chld) {
        head = (**inst).name() + dot + (**inst).get(chld->head()).safeName();
        graph.insertEdge(node, node2attr.first(head));
      }
      // Parents existing in the instance type's DAG
      const ArcSet& prnts  = (**inst).type().dag().parents((**attr).id());
      for (ArcSet::const_iterator prnt = prnts.begin(); prnt != prnts.end(); ++prnt) {
        switch ((**inst).type().get(prnt->tail()).elt_type()) {
          case ClassElement::prm_attribute:
          case ClassElement::prm_aggregate:
            {
              tail = (**inst).name() + dot + (**inst).get(prnt->tail()).safeName();
              graph.insertEdge(node, node2attr.first(tail));
              break;
            }
          case ClassElement::prm_slotchain:
            {
              const Set<Instance*>& ref = (**inst).getInstances(prnt->tail());
              for (Set<Instance*>::const_iterator jnst = ref.begin(); jnst != ref.end(); ++jnst) {
                if (match.exists(*jnst)) {
                  tail = (**jnst).name() + dot + static_cast<const SlotChain&>((**inst).type().get(prnt->tail())).lastElt().safeName();
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
                                     Set<Potential<prm_float>*>& pool,
                                     Set<Potential<prm_float>*>& trash)
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
    trash.insert( bucket_pot );
    pool.insert( bucket_pot );
  }
}


} /* namespace prm */
} /* namespace gum */
// ============================================================================
